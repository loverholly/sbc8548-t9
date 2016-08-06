#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/hwConf.h>
#include "wrSbc8548.h"
#include "semLib.h"
#include "i2cP.h"

#define PRUNTIME(fmt, args...) do {                                     \
        printf("INF:%s(%d)"fmt"\n", __FUNCTION__, __LINE__, ##args);    \
    }while(0)


#define PDEBUG(level, fmt, args...) do {                                    \
        if (level <= DBG_LEVEL) {                                       \
            printf("DBG:%s(%d)"fmt"\n", __FUNCTION__, __LINE__, ##args);    \
        }                                                               \
}while(0)

#define PASSERT(condition, fmt, args...) do {                                    \
        if (condition) {                                       \
            printf("AST:%s(%d)"fmt"\n", __FUNCTION__, __LINE__, ##args);    \
        }                                                               \
}while(0)

#define PERROR(fmt, args...) do {                                    \
        printf("\033[1;31m\033[5m""ERR:%s(%d)"fmt"\n""\033[0m", __FUNCTION__, __LINE__, ##args);    \
    }while(0)
#undef DBG_LEVEL
#define DBG_LEVEL -1

/* 83/85/86通用的i2c寄存器定义 */
typedef struct ppc_fsl_i2c {

	unsigned char adr;		/* I2C slave address */
	unsigned char res0[3];

	unsigned char fdr;		/* I2C frequency divider register */
	unsigned char res1[3];

	unsigned char cr;		/* I2C control redister	*/
	unsigned char res2[3];

	unsigned char sr;		/* I2C status register */
	unsigned char res3[3];

	unsigned char dr;		/* I2C data register */
	unsigned char res4[3];

	unsigned char dfsrr;	/* I2C digital filter sampling rate register */
	unsigned char res5[3];

	/* Fill out the reserved block */
	unsigned char res6[0xE8];
} fsl_i2c_regs_t;

struct fsl_i2c_dev
{
	SEM_ID sem;
	unsigned char   workMode;/*工作模式 master or slave 目前只实现了master模式*/
	unsigned char  slaveAddr;/*作为slave模式的时候，其地址*/
    UINT32 baud;
    volatile fsl_i2c_regs_t * regs;
	unsigned char  fdr;/*I2C frequency divider*/
	unsigned char  dfsrr;/*I2C digital filter sampling rate*/
    
};


#define I2C_ADR			0xFE
#define I2C_ADR_SHIFT	1
#define I2C_ADR_RES		~(I2C_ADR)

#define IC2_FDR			0x3F
#define IC2_FDR_SHIFT	0
#define IC2_FDR_RES		~(IC2_FDR)

#define I2C_CR_MEN		0x80
#define I2C_CR_MIEN		0x40
#define I2C_CR_MSTA		0x20
#define I2C_CR_MTX		0x10
#define I2C_CR_TXAK		0x08
#define I2C_CR_RSTA		0x04
#define I2C_CR_BCST		0x01

#define I2C_SR_MCF		0x80
#define I2C_SR_MAAS		0x40
#define I2C_SR_MBB		0x20
#define I2C_SR_MAL		0x10
#define I2C_SR_BCSTM	0x08
#define I2C_SR_SRW		0x04
#define I2C_SR_MIF		0x02
#define I2C_SR_RXAK		0x01

#define I2C_DR			0xFF
#define I2C_DR_SHIFT	0
#define I2C_DR_RES		~(I2C_DR)

#define I2C_DFSRR		0x3F
#define I2C_DFSRR_SHIFT	0
#define I2C_DFSRR_RES	~(I2C_DR)

#define MAX_FSL_I2C_BUS_NUM 2


LOCAL struct fsl_i2c_dev *fsl_devs[2]={NULL,NULL};/*配置信息保存下来，通讯异常重新配置控制器*/


/** 
 * 初始化i2c总线
 * 
 * @param busIndex 总线号
 * 
 * @return OK(0):成功 <0:失败
 */
LOCAL STATUS fslI2cInit(int Index)
{
	  extern STATUS fslAdvI2cClockSetGet(int busIndex,unsigned char SetOrGet,unsigned int *clockFre);
    struct fsl_i2c_dev *dev = fsl_devs[Index];
    volatile fsl_i2c_regs_t * regs = dev->regs;
    /* disable i2c module  MASTER 模式*/
    regs->cr = (regs->cr & (~I2C_CR_MEN)) | dev->workMode;
    /* set baud rate */
    regs->fdr = dev->fdr;
    /* set slave address */
    regs->adr = dev->slaveAddr;
    /* set master, tx mode */
    regs->cr = dev->workMode |I2C_CR_MTX;
    /* enable module */
    regs->sr = 0x00;
    regs->cr = I2C_CR_MEN;
	

    
    fslAdvI2cClockSetGet(Index, 1, (UINT32 *)&(dev->baud));
    PDEBUG(1, "init done, CR %x SR %x\n", regs->cr, regs->sr);
	
    if(!dev->sem)
        dev->sem = semBCreate(SEM_Q_FIFO, SEM_FULL);

    return OK;
}

/** 
 * 检查是否有ACK信号 check  ACK signal after  transmitted 
 * @param busIndex 总线号
 * @return OK(0):有ACK <0:无ACK
 */
LOCAL STATUS fslI2cACKCheck(int busIndex)
{
    UINT32 now;
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
    now = cpu_timebase_get();
    do{
        UINT8 val;
        val = regs->sr;
        if((val & I2C_SR_MIF)){ /* tx finished */
            PDEBUG(2, "tx finished %dus ", cpu_time_diff_us(now));
            if(!(val & I2C_SR_RXAK)){
                PDEBUG(2, "bus %d ACK\n", busIndex);
                WRS_ASM("sync");
                regs->sr = 0; /* clear int pending */
                return OK;
            }else{
                PERROR("bus %d no ACK\n", busIndex);
                return ERROR;
            }
        }
    }while(cpu_time_diff_us(now) < 10000); /* timeout 10ms */
    PERROR("bus %d timeout in %d us", busIndex, 10000);
    return ERROR;
}

/** 
 * 底层发送start信号
 * @param busIndex 总线号
 * @return OK(0):成功 <0:失败
 */
LOCAL STATUS fslI2cRawStart(int busIndex)
{
    UINT32 now;
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
    /* check the bus busy status */
    now = cpu_timebase_get();
    while((regs->sr & I2C_SR_MBB) && cpu_time_diff_us(now) < 1000){
        PDEBUG(0, "busy reinit %x\n", regs->sr);
        WRS_ASM("sync");
        fslI2cInit(busIndex);
    }
    if((regs->sr & I2C_SR_MBB)){
        PERROR("i2c bus %d busy\n", busIndex);
        return ERROR;
    }
    WRS_ASM("sync");
    regs->cr |= I2C_CR_MSTA | /* start */
        I2C_CR_MTX; /* tx mode */
        /* check bus status */
    return OK;
}


/** 
 * 底层发送restart信号
 * @param busIndex 总线号
 * @return OK(0):成功 <0:失败
 */
LOCAL STATUS fslI2cRawReStart(int busIndex)
{
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
	
    regs->cr |= I2C_CR_RSTA | /* restart */
        					 I2C_CR_MTX |  /*tx*/
        					 I2C_CR_MSTA | I2C_CR_MEN; /* tx mode */
    return OK;

}


/** 
 * 底层发送stop信号
 * @param busIndex 总线号
 * @return OK(0):成功 <0:失败
 */
LOCAL STATUS fslI2cRawStop(int busIndex)
{
    UINT32 now;
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
	
    regs->cr &= ~(I2C_CR_MSTA | I2C_CR_TXAK | I2C_CR_MTX);
    WRS_ASM("sync");

	/* check bus status */
    now = cpu_timebase_get();
    do{
        if(0 == (regs->cr & I2C_SR_MBB)){
            PDEBUG(2, "bus %d stop ok", busIndex);
            return OK;
        }
    }while(cpu_time_diff_us(now) < 1000); /* timeout 1ms */
    PERROR("i2c %d stop timeout\n", busIndex);
    return ERROR;

}


/** 
 * 底层读取1Byte
 * 
 * @param busIndex 总线号
 * @param pData 数据指针
 * @param ack 是否检查ACK,1:检查  0:不检查
 * 
 * @return OK(0):成功 <0:失败
 */
LOCAL STATUS fslI2cRawRead(int busIndex, char * pData, int ack)
{
    UINT32 now;
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
	
    /* set ACK */
    if (!ack){
        /* Don't send master ack. */
        regs->cr = (regs->cr & (~I2C_CR_MTX)) | I2C_CR_TXAK;
        PDEBUG(2, "bus %d read without ack", busIndex);
    }else{
        /* Send master ack. */
        regs->cr = regs->cr & (~I2C_CR_MTX | I2C_CR_TXAK);
        PDEBUG(2, "bus %d read with ack", busIndex);
    }
    WRS_ASM("sync");
        /* 读数据 */
    *pData = regs->dr;
    WRS_ASM("sync");
        /* 等待数据读完 */
    now = cpu_timebase_get();
    do{
        if((regs->sr & I2C_SR_MIF) && (regs->sr & I2C_SR_MCF)){ /* read finish */
            regs->sr = 0;
            PDEBUG(2, "read cost %dus\n", cpu_time_diff_us(now));
            return OK;
        }
    }while(cpu_time_diff_us(now) < 1000); /* timeout 1ms */
    return ERROR;
}

/** 
 * 底层写入1Byte
 * @param busIndex 总线号
 * @param data 写入的数据内容
 * @return OK(0):成功 <0:失败
 */
LOCAL STATUS fslI2cRawWrite(int busIndex, UINT8 data)
{
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
	
    regs->dr = data;
	
    return fslI2cACKCheck(busIndex);
}

/***********************************************************************
 *******************************接口封装 ********************************
************************************************************************/
#if 1
extern int abs();
static int i2cDiv[]={
	384,416,480,576,640,704,832,1024,1152,1280,1536,1920,2304,2560,
	3072,3840,4608,5120,6144,7680,9216,10240,12288,15360,18432,20480,
	24576,30720,36864,40960,49152,61440,256,288,320,352,384,448,512,
	576,640,768,896,1024,1280,1536,1792,2048,2560,3072,3584,4096,5120,
	6144,7168,8192,10240,12288,14336,16384,20480,24576,28672,32768
};
STATUS fslAdvI2cClockSetGet(int busIndex,unsigned char SetOrGet,unsigned int *clockFre)
{
	int i2cDivValeT;
	unsigned char i;
	unsigned char minFlag,minFlagT;
    struct fsl_i2c_dev *dev= fsl_devs[busIndex];
    volatile fsl_i2c_regs_t * regs = dev->regs;
	if(SetOrGet){/*查找最接近的DIV 值*/
		i2cDivValeT=(sysClkFreqGet()/(*clockFre))/2;
		minFlag=0;
		for(i=0;i<(sizeof(i2cDiv)/sizeof(i2cDiv[0]));i++){
			minFlagT = (abs(i2cDiv[i]-i2cDivValeT) >= abs(i2cDiv[i+1]-i2cDivValeT)) ? 
							(i+1) : i;
			minFlag  = (abs(i2cDiv[minFlag]-i2cDivValeT) >= abs(i2cDiv[minFlagT]-i2cDivValeT)) ?
					   minFlagT : minFlag;
		}
		regs->fdr=minFlag;
		fsl_devs[busIndex]->fdr=minFlag;
	}else{
		i2cDivValeT=regs->fdr;
		*clockFre=(sysClkFreqGet()/(i2cDiv[i2cDivValeT]))/2;
	}

	return 0;
		
}
#endif

/** 
 * QUICC系列CPU读取I2C总线
 * 
 * @param busIndex 总线ID
 * @param devAddr 设备地址
 * @param regAddr 寄存器地址
 * @param pData 数据buffer指针
 * @param len 数据buffer长度(单位Byte)
 * 
 * @return OK(0):成功  <0:失败
 */
STATUS fslI2cRead(int busIndex, UINT8 devAddr, UINT8 regAddr, char * pData, int len)
{
	int left;
    struct fsl_i2c_dev *dev = fsl_devs[busIndex];
	
	semTake(dev->sem, WAIT_FOREVER);
		/* start */
	if(OK != fslI2cRawStart(busIndex)){
		PERROR("bus %d start failed", busIndex);
		goto errorOpr;
	}
		/*write dev addr */
	if(OK != fslI2cRawWrite(busIndex, devAddr << 1 )){
		PERROR("bus %d devaddr failed", busIndex);
		goto errorOpr;
	}
	PDEBUG(2, "bus %d devaddr writed", busIndex);

		/* write reg addr */
	if(OK != fslI2cRawWrite(busIndex, regAddr )){
		PERROR("bus %d reg addr failed", busIndex);
		goto errorOpr;
	}
	PDEBUG(2, "bus %d regAddr", busIndex);
	
		/* restart */
	if(OK != fslI2cRawReStart(busIndex)){
		PERROR("bus %d restart failed\n", busIndex);
		goto errorOpr;
	}
		/* dev addr read */
	if(OK != fslI2cRawWrite(busIndex, (devAddr << 1)|1 )){
		PERROR("bus %d write failed\n", busIndex);
		goto errorOpr;
	}
	PDEBUG(2, "bus %d write devaddr for read ok", busIndex);
	
	 /* I2C读取实际是触发一次读取操作，fslI2cRawRead的ack参数决定了读取结束时是否会发出ACK
		 * 实际的数据获取是在下一次fslI2cRawRead
		 * 比如读三个数据，先把两个带ack的读命令，再发两个不带ack的读命令，第2-4次读取可以获取3个byte
		 */
		/* dummy read first */
	left = len;
	fslI2cRawRead(busIndex, pData, (--left)?1:0);
	PDEBUG(2, "dummy %x\n", pData[0]);
		/* read left bytes */
	while((left)){
		fslI2cRawRead(busIndex, pData++, (--left)?1:0);
	}
		/* 读取最后一个字节不带ACK */
	fslI2cRawRead(busIndex, pData, 0);
	fslI2cRawStop(busIndex);
	/*实际使用中,当连续读取的时候，会出现start覆盖stop的时序
	，实际时序看不到stop信号，所以这里需要保证stop被完全发
	送出去*/
	{
		UINT32 now,i2cFrequence;
	    now = cpu_timebase_get();
		fslAdvI2cClockSetGet(busIndex,0,(UINT32 *)&i2cFrequence);
		while(cpu_time_diff_us(now)<=(1000000/i2cFrequence));
	}
	
	semGive(dev->sem);
	return OK;
  errorOpr:
	fslI2cRawStop(busIndex);
		/* 重初始化i2c硬件模块以从错误状态中恢复 */
	fslI2cInit(busIndex);
	semGive(dev->sem);
	return ERROR;
	
}
/** 
 * QUICC系列CPU读取I2C总线，高级用法
 * 
 * @param busIndex 总线ID
 * @param devAddr 设备地址
 * @param regAddrArray 寄存器地址
 * @param regAddrLen 寄存器地址的长度，单位byte
 * @param pData 数据buffer指针
 * @param len 数据buffer长度(单位Byte)
 * @param waitRtn 某些i2c设备在写完寄存器地址后需要等待固定的时间或是查询某个状态，waitRtn可以实现这种操作
 * @return OK(0):成功  <0:失败
 */
STATUS fslAdvI2cRead(int busIndex, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn)
{
    int left;

	struct fsl_i2c_dev *dev = fsl_devs[busIndex];
	/*必要的检查*/
	semTake(dev->sem, WAIT_FOREVER);
    /* start */
    if(OK != fslI2cRawStart(busIndex)){
        PERROR("bus %d start failed", busIndex);
        goto errorOpr;
    }
    /*write dev slave addr */
    if(OK != fslI2cRawWrite(busIndex, devAddr << 1 )){
        PERROR("bus %d devaddr failed", busIndex);
        goto errorOpr;
    }
    PDEBUG(2, "bus %d devaddr writed", busIndex);
    /* write reg addr */
    for(left=0; left<regAddrLen; left++){
        if(OK != fslI2cRawWrite(busIndex, regAddrArray[left] )){
            PERROR("bus %d reg addr index %d, value 0x%x failed", busIndex, left, regAddrArray[left]);
            goto errorOpr;
        }
    }
    PDEBUG(2, "bus %d regAddr", busIndex);
    /* 如果有waitRtn定义，执行之 */
    if((waitRtn)){
        if(OK!= waitRtn(busIndex, devAddr, regAddrArray, regAddrLen, pData, len)){
            PERROR("bus %d waitRtn failed", busIndex);
            goto errorOpr;
        }
    }
    /* restart */
    if(OK != fslI2cRawReStart(busIndex)){
        PERROR("bus %d restart failed\n", busIndex);
        goto errorOpr;
    }
    /* dev slave addr read */
    if(OK != fslI2cRawWrite(busIndex, (devAddr << 1)|1 )){
        PERROR("bus %d write failed\n", busIndex);
        goto errorOpr;
    }
    PDEBUG(2, "bus %d write devaddr for read ok", busIndex);
    
    /* I2C读取实际是触发一次读取操作，fslI2cRawRead的ack参数决定了读取结束时是否会发出ACK
        * 实际的数据获取是在下一次fslI2cRawRead
        * 比如读三个数据，先把两个带ack的读命令，再发两个不带ack的读命令，第2-4次读取可以获取3个byte
        */
    /* dummy read first */
    left = len;
    fslI2cRawRead(busIndex, pData, (--left)?1:0);
    PDEBUG(2, "dummy %x\n", pData[0]);
    /* read left bytes */
    while((left)){
        fslI2cRawRead(busIndex, pData++, (--left)?1:0);
    }
    /* 读取最后一个字节不带ACK */
    fslI2cRawRead(busIndex, pData, 0);
    fslI2cRawStop(busIndex);
	/*实际使用中,当连续读取的时候，会出现start覆盖stop的时序
	，实际时序看不到stop信号，所以这里需要保证stop被完全发
	送出去*/
	{
		UINT32 now,i2cFrequence;
	    now = cpu_timebase_get();
		fslAdvI2cClockSetGet(busIndex,0,(UINT32 *)&i2cFrequence);
		while(cpu_time_diff_us(now)<=(1000000/i2cFrequence));
	}
	
    semGive(dev->sem);
	
    return OK;
  errorOpr:
    fslI2cRawStop(busIndex);
        /* 重初始化i2c硬件模块以从错误状态中恢复 */
    fslI2cInit(busIndex);
    semGive(dev->sem);
    return ERROR;
    
}

/** 
 * QUICC系列CPU写入I2C总线
  * @param busIndex 总线ID
 * @param devAddr 设备地址
 * @param regAddr 寄存器地址
 * @param pData 数据buffer指针
 * @param len 数据buffer长度(单位Byte)
 * @return OK(0):成功  <0:失败
 */
STATUS fslI2cWrite(int busIndex, UINT8 devAddr, UINT8 regAddr, char * pData, int len)
{
    int i;
	
    struct fsl_i2c_dev *dev = fsl_devs[busIndex];
	semTake(dev->sem, WAIT_FOREVER);
        /* start */
    if(OK != fslI2cRawStart(busIndex)){
        printf("start failed\n");
        goto errorOpr;
    }
        /* dev addr */
    if(OK != fslI2cRawWrite(busIndex, devAddr << 1 )){
        printf("devaddr failed\n");
        goto errorOpr;
    }
        /* reg addr */
    if(OK != fslI2cRawWrite(busIndex, regAddr )){
        printf("reg addr failed\n");
        goto errorOpr;
    }
	/*写入数据*/
    for(i=0; i<len; i++){
        if(OK != fslI2cRawWrite(busIndex, pData[i] )){
            printf("write data%d  failed\n", i);
            goto errorOpr;
        }
    }
    fslI2cRawStop(busIndex);
	/*实际使用中,当连续发送的时候，会出现start覆盖stop的时序
	，实际时序看不到stop信号，所以这里需要保证stop被完全发
	送出去*/
	{
		UINT32 now,i2cFrequence;
	    now = cpu_timebase_get();
		fslAdvI2cClockSetGet(busIndex,0,(UINT32 *)&i2cFrequence);
		while(cpu_time_diff_us(now)<=(1000000/i2cFrequence));
	}
	
    semGive(dev->sem);
    return OK;
  errorOpr:
    fslI2cRawStop(busIndex);
        /* 重初始化i2c硬件模块以从错误状态中恢复 */
    fslI2cInit(busIndex);
    semGive(dev->sem);
    return ERROR;
    
}
/** 
 * QUICC系列CPU写入I2C总线，高级用法
 * 
 * @param busIndex 总线ID
 * @param devAddr 设备地址slave addr
 * @param regAddrArray 寄存器地址
 * @param regAddrLen 寄存器地址的长度，单位byte
 * @param pData 数据buffer指针
 * @param len 写入的数据buffer长度(单位Byte)
 * @param waitRtn 某些i2c设备在写完寄存器地址后需要等待固定的时间或是查询某个状态，waitRtn可以实现这种操作
 * @return OK(0):成功  <0:失败
 */
STATUS fslAdvI2cWrite(int busIndex, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn)
{
    int i;
    struct fsl_i2c_dev *dev = fsl_devs[busIndex];

	semTake(dev->sem, WAIT_FOREVER);
    /* start */
    if(OK != fslI2cRawStart(busIndex)){
        printf("start failed\n");
        goto errorOpr;
    }
    /* dev slave addr */
    if(OK != fslI2cRawWrite(busIndex, devAddr << 1 )){
        printf("devaddr failed\n");
        goto errorOpr;
    }
    /* reg addr */
    for(i=0; i<regAddrLen; i++){
        if(OK != fslI2cRawWrite(busIndex, regAddrArray[i])){
            printf("reg addr failed\n");
            goto errorOpr;
        }
    }
	/*写入数据*/
    for(i=0; i<len; i++){
        if(OK != fslI2cRawWrite(busIndex, pData[i] )){
            printf("write data%d  failed\n", i);
            goto errorOpr;
        }
    }
	/*stop*/
    fslI2cRawStop(busIndex);

	/*实际使用中,当连续发送的时候，会出现start覆盖stop的时序
	，实际时序看不到stop信号，所以这里需要保证stop被完全发
	送出去*/
	{
		UINT32 now,i2cFrequence;
	    now = cpu_timebase_get();
		fslAdvI2cClockSetGet(busIndex,0,(UINT32 *)&i2cFrequence);
		while(cpu_time_diff_us(now)<=(1000000/i2cFrequence));
	}
	
    semGive(dev->sem);
	
    return OK;
  errorOpr:
    fslI2cRawStop(busIndex);
    /* 重初始化i2c硬件模块以从错误状态中恢复 */
    fslI2cInit(busIndex);
    semGive(dev->sem);
    return ERROR;
 
}
/**
 * @page page_i2c_fsl_cfg_demo PowerPC QUICC系列I2C控制器
 * PowerPC 83/85/86/P10XX/P20XX及以上片内集成的I2C控制器
 * 设备定义如下:
 @code 
struct hcfResource i2cx0Resources[] =
 {
    { VXB_REG_BASE,     HCF_RES_ADDR,  {(void *)((UINT32)CCSBAR | 0x3000)} },
    { "baudRate",       HCF_RES_INT,   {(void *)400000} },
};

#define i2cx0ResourcesNum NELEMENTS(i2cx0Resources)
struct hcfDevice hcfDeviceList[] = 
{
    ...
    {  "I2C",     0, VXB_DEVID_FSL_I2C,   0, i2cx0ResourcesNum,  i2cx0Resources},
    ...
};
@endcode*/
/**

 * I2C配置方式为在工程所在的deviceConfig.c中定义设备，每一个16C系列的UART设备都存在一个定义表与之对应\n
 * hcfDeviceList表中定义的参数详见@ref hcfResource
 * 多个I2C设备的标志为i2cRead/i2cWrite调用时传入的控制器ID，每一个控制器都有一个全局唯一的编号。从0开始编号
 * - (struct hcfDevice *)->devName:目前无用，使用"i2c"即可
 * - (struct hcfDevice *)->devUnit:该I2C控制器在系统中的编号，每一个设备都应该有一个唯一的编号。
 * - (struct hcfDevice *)->busType:QUICC系列的I2C控制器ID为VXB_DEVID_FSL_I2C
 * - (struct hcfDevice *)->busIndex:，I2C驱动该字段无意义
 * - (struct hcfDevice *)->count: I2C控制器配置表配置项个数，上例中为i2cx0ResourcesNum
 * - (struct hcfDevice *)->pResource:配置表指针，上例为i2cx0Resources
 * 
 * 可配置的参数列表如下
 * 
 * 配置名            | 类型             | 必要 |   说明
 * -----------      | --------------- | --  | ------------------------------
 * regBase          | HCF_RES_ADDR    | Y  | 设备的基地址，16C55X某一个DUART通道占用8Byte地址，因此该地址应该是8Byte地址对齐的
 * "defBaudrate"    | HCF_RES_INT     | N   | I2C接口默认的波特率，单位为bit/s，如果不配置默认为400KHz
 * 最常用的I2C波特率为400KHz，FSL的中断形式为每次传输一个byte，byte传输完后触发中断。一个byte传输的时间量级为20-30us。\n
 * 而中断本身的时间开销也需要10us这个量级。因此，在400KHz波特率下，中断模式与非查询模式的差异并不大。\n
 * 当使用较低波特率时，中断的优势才能体现出来。
 */
int fsl_i2c_probe(struct hcfDevice * pDevice)
{
	int i=0;
    struct i2c_routine rtns = {
        fslAdvI2cRead, 
        fslAdvI2cWrite,
        NULL
    };
    i = pDevice->devUnit;
	fsl_devs[i]=(struct fsl_i2c_dev *)malloc(sizeof(struct fsl_i2c_dev));
    memset(fsl_devs[i], 0, sizeof(*fsl_devs[i]));
    
    if (OK != devResourceGet(pDevice, VXB_REG_BASE, HCF_RES_ADDR, (void *)&(fsl_devs[i]->regs))){
        PERROR("FSL I2C控制器%d无基址定义", pDevice->devUnit);
        return ERROR;
    }
	if (OK != devResourceGet(pDevice, "baudRate", HCF_RES_INT, (void *)&(fsl_devs[i]->baud))){
        fsl_devs[i]->baud=400000; /*默认值波特率400Kbps*/
    }
    i2cRegister(pDevice->devUnit, pDevice->devUnit, &rtns);
    return fslI2cInit(i);
}


STATUS i2cStatusShow(int index)
{
	unsigned int i2cFre;
    struct fsl_i2c_dev *dev= fsl_devs[index];
    volatile fsl_i2c_regs_t * regs = dev->regs;
	printf("\ni2c controller[%d] REG status\n",index);
	
	printf("\nI2CCR:0x%x\n",regs->cr);
	printf("\nI2FDR:0x%x\n",regs->fdr);
	printf("\nI2ADR:0x%x\n",regs->adr);
	printf("\nI2CSR:0x%x\n",regs->sr);

	fslAdvI2cClockSetGet(index,0,(unsigned int *)&i2cFre);

	printf("\ncontroller input FRE:%dMHZ  output FRE:%dKHZ\n",
		sysClkFreqGet()/1000000,i2cFre/1000);

	return 0;
}

struct hcfResource i2cx0Resources[] =
{
    { VXB_REG_BASE,   HCF_RES_ADDR,  {(void *)(CCSBAR + 0x3000)} },/*控制器基址*/
    { "baudRate",       HCF_RES_INT,   {(void *)400000} }, /*默认波特率*/
};
#define i2cx0Num NELEMENTS(i2cx0Resources)

struct hcfResource i2cx1Resources[] =
{
    { VXB_REG_BASE,   HCF_RES_ADDR,  {(void *)(CCSBAR + 0x3100)} },/*控制器基址*/
    { "baudRate",       HCF_RES_INT,   {(void *)400000} }, /*默认波特率*/
};
#define i2cx1Num NELEMENTS(i2cx1Resources)

const struct hcfDevice fslI2cDev[]={
	{ "fslI2c", 0, VXB_BUSID_PLB, 0, i2cx0Num, i2cx0Resources},	
	{ "fslI2c", 1, VXB_BUSID_PLB, 0, i2cx1Num, i2cx1Resources},
};

void fsl_i2c_init(void)
{
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		fsl_i2c_probe(&fslI2cDev[i]);
	}
}
