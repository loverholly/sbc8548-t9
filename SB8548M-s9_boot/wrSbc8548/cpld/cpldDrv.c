#include <vxWorks.h>
#include "errno.h"
#include "config.h"
#include "cpldDrv.h"
#include <string.h>
#include <logLib.h>
#include <stdio.h>
#include <intLib.h>
#include <wdLib.h>
#include "cpldDrv.h"
#include <taskLib.h>


#if !defined(STACK_DSP) && defined(PRJ_BUILD)
/* vx环境，而且在os里面，中断可以自动重复 */
#define CPLD_INTERRUPT_AUTO_REPEAT
#endif



/* 定义依据CPU小系统设计规范版,其bit定义仅适用于PPC这种大端CPU */
struct cpld_rmap{
        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!大端寄存器!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */   
    /* offset0:PCB版本寄存器 */
    unsigned char pcb_ver_major:4;  
    unsigned char pcb_ver_minor:4;    
    /* offset1:CPLD版本寄存器 */
    unsigned char cpld_ver_major:4;  
    unsigned char cpld_ver_minor:4;    
        /* offset2:控制寄存器 */
    unsigned char wd_int_pend:1;    /* 看门狗模式 0:超时时产生中断后0.5s后复位，1:超时时仅产生中断，不复位 */
    unsigned char wd_rst_enable:1;    
    unsigned char bootrom_protect_off:1; /* CS0上的flash的写保护开关 */
    unsigned char flash_protect_off:1;   /* CS1上的flash写保护开关 */
    unsigned char led0_off:1;   /* 1:off, 0:on */
    unsigned char led1_off:1;   /* 1:off, 0:on */
    unsigned char jp2_off:1;   /* 1:off, 0:on */
    unsigned char jp1_off:1;   /* 1:off, 0:on */
        /* offset3:看门狗喂狗寄存器，写入的时间就是超时时间 */
    unsigned char wd_timeout;   /* timeout x 10ms */
        /* offset4: */
    union{
        struct{
            unsigned char cpu_id:2;
            unsigned char rsv0:2;
            unsigned char fpga_exist:1; /* 是否存在额外的FPGA寄存器 */
            unsigned char boot_mode:3;
        }w;
        unsigned char reset_word;             /* 复位控制寄存器 */
    }reg4;
        /* offset5: 测试寄存器*/
    unsigned char test;
        /* offset6: 小系统规范版本寄存器*/
    unsigned char standard_version_major:4;
    unsigned char standard_version_minor:4;
    unsigned char rsv_regs[4];  /* reg 0x7-0xa, 7, 8, 9, 0xa */
        /* offset 0xb */
    union {
        struct{
            unsigned char tp_flag:1; /* 状态寄存器，只读 1:温度超限 0:正常 */
            unsigned char wd_flag:1;
            unsigned char rsv0:1;
            unsigned char tp_mask:1;
            unsigned char wd_mask:1;
            unsigned char rsv1:1;
            unsigned char mode:2; /**<  2'b00:仅触发中断
                                   *    2'b01:触发中断并在0.5s后复位整板
                                   *    2'b10:保留
                                   *    2'b11:硬件复位发生后的默认值。表示软件尚未配置温度检测模块*/
        }w;
        unsigned char byte;
    }temperature;
    unsigned char rsv1_regs[3];
        /* offset 0xf */
    unsigned char addr_ext;     /* 扩展地址寄存器 */
};
#ifdef INCLUDE_ZMD_MINI_SYSTEM
static int m_cpld_init=1;
#else
static int m_cpld_init=0;
#endif
#ifdef CPLD_INTERRUPT_AUTO_REPEAT
/* CPLD小系统使用的vxworks watchdog,目前只有vxworks有效 */
WDOG_ID __cpld_wd = NULL;
#endif

#define CPLD_INIT_CHECK(ret) do{if(m_cpld_init==0) return (ret);}while(0)
#define CPLD_INIT_CHECK_NONE() do{if(m_cpld_init==0) return ;}while(0)




static UINT32 watchDogDefaultLoadVal = 0xff;
int watchDogMode = WATCHDOG_ENABLE_RESET;





/** 
 * 根据传入参数，强制设置CPLD是否存在.在KBOOT刚启动时，是否有CPLD是不能通过platformBoardName来判断的，此时必须强制
 * kboot中使用,应该传递为1，如果不是按小系统规范做，可以传为0 
 * 
 * @param cpld_exist 1:遵循小系统规范  0:不遵循小系统规范
 * 
 * @return 
 */
int cpld_init(int cpld_exist)
{
    m_cpld_init=cpld_exist;
    return m_cpld_init;
    
}

/* 该函数需要系统组建初始化后再调用的，放在zmDriverInit函数中调用 */
void cpld_os_post_init()
{
#ifdef CPLD_INTERRUPT_AUTO_REPEAT
    __cpld_wd = wdCreate();
#endif
}


/* 获取PCB major version */
int pcb_ver_major()
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return cpld->pcb_ver_major;
}

int pcb_ver_minor()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return cpld->pcb_ver_minor;
    
}

int cpld_ver_major()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return cpld->cpld_ver_major;
}

int cpld_ver_minor()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return cpld->cpld_ver_minor;
}

/** 
 * 检查CPLD是否就绪
 * @return TRUE(1):就绪   FALSE(0):未就绪
 */
BOOL cpld_ready()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    int i;
    for(i=0; i<8; i++){
        cpld->test = 1 << i;
        VX_MEM_BARRIER_RW();
        if(cpld->test != (unsigned char)(~(1<<i))){
            return FALSE;
        }
    }
    return TRUE;
}

/* 获取当前bootflash写保护状态
 * return 
 *     1: 写保护
 *     0: 关闭写保护
 */
int bootFlashWPStsGet()
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return !cpld->bootrom_protect_off;
}


/* 设置获取bootflash写保护状态
 * 参数：
 *     enable
 *         1: 打开写保护
 *         0: 关闭写保护
 */
void bootFlashWPSet(int enable)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    if ((enable)){
        cpld->bootrom_protect_off = 0;
    }else{
        cpld->bootrom_protect_off = 1;
    }
}


/* 设置获取数据flash(连接在CS1上)写保护状态
 * 参数：
 *     enable
 *         1: 打开写保护
 *         0: 关闭写保护
 */
void mainFlashWPSet(int enable)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    if ((enable)){
        cpld->flash_protect_off = 0;
    }else{
        cpld->flash_protect_off = 1;
    }
}






/* 调试LED控制
 * parameters:
 *      id:LED index
 *      on: 1(led亮)    0(LED灭)
 */
void DbgLEDCtrl(int id, int on)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    if (!id){
        cpld->led0_off = !on;
    }else if(id == 1){
        cpld->led1_off = !on;
    }
}
#if 0
void cpuFreqRegSet(unsigned char val)
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    cpld->freq_cfg = val;
}


unsigned char cpuFreqRegGet()
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return  cpld->freq_cfg;
}
#endif




/**
 * 硬复位CPU
 */
void cpuHardReset()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    int i;
#ifdef PRJ_BUILD
    int flag;
    flag = intLock();
#else
#endif
    for(i=0; i<100; i++){
        cpld->reg4.reset_word = 0xa5;
        VX_MEM_BARRIER_RW();
        cpld->reg4.reset_word = 0x59;
        VX_MEM_BARRIER_RW();
    }
#ifdef PRJ_BUILD
    intUnlock(flag);
#endif
}



/** 获取CPU ID */
int cpld_cpu_id()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return cpld->reg4.w.cpu_id;
}

int cpuGetBootType()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return cpld->reg4.w.boot_mode;
}




void cpldWrite(int offset, int bitWidth, unsigned int value)
{
    switch(bitWidth){
        case 4:
            *(volatile unsigned int *)(CPLD_REG_BASE + offset) = value;
            break;
        case 2:
            *(volatile unsigned short *)(CPLD_REG_BASE + offset) = value;
            break;
        case 1:
        default:
            *(volatile unsigned char *)(CPLD_REG_BASE + offset) = value;
            break;
            
    }
}

unsigned int cpldRead(int offset, int bitWidth)
{
    switch(bitWidth){
        case 4:
            return *(volatile unsigned int *)(CPLD_REG_BASE + offset);
        case 2:
            return *(volatile unsigned short *)(CPLD_REG_BASE + offset);
        case 1:
        default:
            return *(volatile unsigned char *)(CPLD_REG_BASE + offset);
    }
}




/*********************************************************************************
 *                               看门狗驱动代码                                     *
 *********************************************************************************/
/* 看门狗计数的每一个tick的时长，默认定义为40ms */
#define CPLD_WATCHDOG_TICK_LEN 40

/* 获取当前看门狗模式
 * return 详见@ref WATCHDOG_MODE_T
 */
WATCHDOG_MODE_T watchDogStsGet()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return (WATCHDOG_MODE_T)cpld->wd_rst_enable;
}





/** 
 * 写入看门狗寄存器
 * 
 * @param tickNum 超时的tick数
 */
void WatchDog_regWrite(UINT32 tickNum)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
	
    if(tickNum > 255)
        tickNum = 255;
    cpld->wd_timeout = tickNum;    /* watchdog time out CPLD_WATCHDOG_TICK_LEN * tickNum */
}



/* 关闭看门狗，关闭中断，将看门狗模式强制设置为仅触发中断模式。
 * 参数:无
 * 返回值:无
 */
void WatchDog_Disable()
{
	
	volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
        /* 老版小系统的wwd_rst_enable是看门狗的使能位 */
    cpld->wd_rst_enable = WATCHDOG_DISABLE_RESET;
}



/** 
 * 使能看门狗
 * 挂接WatchDog_Init中传入的回调函数，打开中断，并设置看门狗模式为WatchDog_Init中设置的模式。
 */
void WatchDog_Enable(void)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
        /* 打开看门狗前先喂一次狗 */
    WatchDog_Feed();
    cpld->wd_rst_enable = WATCHDOG_ENABLE_RESET;
        /* 打开看门狗后再喂一次狗 */
    WatchDog_Feed();
}



/** 
 * 喂狗，功能同@WatchDog_TimeOut
 * 
 * @param time_ms 超时的时间，单位为ms
 */
void WatchDog_Load(UINT32 time_ms)
{
    if(time_ms < CPLD_WATCHDOG_TICK_LEN)
        time_ms = CPLD_WATCHDOG_TICK_LEN;
    WatchDog_regWrite(time_ms/CPLD_WATCHDOG_TICK_LEN - 1);
}

/** 
 * 喂狗，功能同@WatchDog_Load
 * @param time_ms 超时的时间，单位为ms
 */
void WatchDog_TimeOut(UINT32 time_ms)
{
    return WatchDog_Load(time_ms);
}



void watchdog_test(int interval, int timeout)
{
    unsigned int now;
    WatchDog_Enable();
    while(1){
        printf(".");
        WatchDog_Load(timeout);
        now = cpu_timebase_get();
        while(cpu_time_diff_us(now) < (UINT32)interval * 1000);
    }
}

/** 
 * 设置温度保护模块工作模式
 * @param mode 工作模式，详见@ref TEMP_PROT_MODE
 * @return 
 */
int temperatureProtectModeSet(TEMP_PROT_MODE mode)
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    if(mode == TEMP_PROT_INT_ONLY){
        cpld->temperature.w.mode = 0; /* 仅触发中断 */
    }else{
        cpld->temperature.w.mode = 1; /* 触发复位并复位 */
    }
    return OK;
}



/** 
 * 喂狗，按照默认超时或WatchDog_Init传入的时间喂狗
 */
void WatchDog_Feed()
{
    WatchDog_regWrite(watchDogDefaultLoadVal);
}


/** 
 * 设置看门狗的工作模式
 * @param mode 详见@ref WATCHDOG_MODE_T
 * @return OK(0):成功 ERROR(-1):失败
 */
int WatchDog_ModeSet(WATCHDOG_MODE_T mode)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    if(mode != WATCHDOG_DISABLE_RESET)
        mode = WATCHDOG_ENABLE_RESET;
    watchDogMode = mode;
    cpld->wd_rst_enable = mode;
    return OK;
}


/** 
 * 初始化看门狗
 * @param mode 看门狗工作模式，详见@ref WATCHDOG_MODE_T
 * @param defaultTimeOutMs 默认超时时间，单位ms
 * @param wdCb 中断触发的回调
 * @param arg 触发的回调函数传递的参数
 * 
 * @return OK(0):成功  ERROR(-1):失败
 */
int WatchDog_Init(WATCHDOG_MODE_T mode, int defaultTimeOutMs, FUNCPTR wdCb, UINT32 arg)
{
    WatchDog_ModeSet(mode);
    if(defaultTimeOutMs > 256*CPLD_WATCHDOG_TICK_LEN)
        defaultTimeOutMs = 256*CPLD_WATCHDOG_TICK_LEN;
    if(defaultTimeOutMs < CPLD_WATCHDOG_TICK_LEN)
        defaultTimeOutMs = CPLD_WATCHDOG_TICK_LEN;
    watchDogDefaultLoadVal = defaultTimeOutMs / CPLD_WATCHDOG_TICK_LEN - 1;
    WatchDog_Enable();
    return OK;
}
