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
/* vx������������os���棬�жϿ����Զ��ظ� */
#define CPLD_INTERRUPT_AUTO_REPEAT
#endif



/* ��������CPUСϵͳ��ƹ淶��,��bit�����������PPC���ִ��CPU */
struct cpld_rmap{
        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!��˼Ĵ���!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */   
    /* offset0:PCB�汾�Ĵ��� */
    unsigned char pcb_ver_major:4;  
    unsigned char pcb_ver_minor:4;    
    /* offset1:CPLD�汾�Ĵ��� */
    unsigned char cpld_ver_major:4;  
    unsigned char cpld_ver_minor:4;    
        /* offset2:���ƼĴ��� */
    unsigned char wd_int_pend:1;    /* ���Ź�ģʽ 0:��ʱʱ�����жϺ�0.5s��λ��1:��ʱʱ�������жϣ�����λ */
    unsigned char wd_rst_enable:1;    
    unsigned char bootrom_protect_off:1; /* CS0�ϵ�flash��д�������� */
    unsigned char flash_protect_off:1;   /* CS1�ϵ�flashд�������� */
    unsigned char led0_off:1;   /* 1:off, 0:on */
    unsigned char led1_off:1;   /* 1:off, 0:on */
    unsigned char jp2_off:1;   /* 1:off, 0:on */
    unsigned char jp1_off:1;   /* 1:off, 0:on */
        /* offset3:���Ź�ι���Ĵ�����д���ʱ����ǳ�ʱʱ�� */
    unsigned char wd_timeout;   /* timeout x 10ms */
        /* offset4: */
    union{
        struct{
            unsigned char cpu_id:2;
            unsigned char rsv0:2;
            unsigned char fpga_exist:1; /* �Ƿ���ڶ����FPGA�Ĵ��� */
            unsigned char boot_mode:3;
        }w;
        unsigned char reset_word;             /* ��λ���ƼĴ��� */
    }reg4;
        /* offset5: ���ԼĴ���*/
    unsigned char test;
        /* offset6: Сϵͳ�淶�汾�Ĵ���*/
    unsigned char standard_version_major:4;
    unsigned char standard_version_minor:4;
    unsigned char rsv_regs[4];  /* reg 0x7-0xa, 7, 8, 9, 0xa */
        /* offset 0xb */
    union {
        struct{
            unsigned char tp_flag:1; /* ״̬�Ĵ�����ֻ�� 1:�¶ȳ��� 0:���� */
            unsigned char wd_flag:1;
            unsigned char rsv0:1;
            unsigned char tp_mask:1;
            unsigned char wd_mask:1;
            unsigned char rsv1:1;
            unsigned char mode:2; /**<  2'b00:�������ж�
                                   *    2'b01:�����жϲ���0.5s��λ����
                                   *    2'b10:����
                                   *    2'b11:Ӳ����λ�������Ĭ��ֵ����ʾ�����δ�����¶ȼ��ģ��*/
        }w;
        unsigned char byte;
    }temperature;
    unsigned char rsv1_regs[3];
        /* offset 0xf */
    unsigned char addr_ext;     /* ��չ��ַ�Ĵ��� */
};
#ifdef INCLUDE_ZMD_MINI_SYSTEM
static int m_cpld_init=1;
#else
static int m_cpld_init=0;
#endif
#ifdef CPLD_INTERRUPT_AUTO_REPEAT
/* CPLDСϵͳʹ�õ�vxworks watchdog,Ŀǰֻ��vxworks��Ч */
WDOG_ID __cpld_wd = NULL;
#endif

#define CPLD_INIT_CHECK(ret) do{if(m_cpld_init==0) return (ret);}while(0)
#define CPLD_INIT_CHECK_NONE() do{if(m_cpld_init==0) return ;}while(0)




static UINT32 watchDogDefaultLoadVal = 0xff;
int watchDogMode = WATCHDOG_ENABLE_RESET;





/** 
 * ���ݴ��������ǿ������CPLD�Ƿ����.��KBOOT������ʱ���Ƿ���CPLD�ǲ���ͨ��platformBoardName���жϵģ���ʱ����ǿ��
 * kboot��ʹ��,Ӧ�ô���Ϊ1��������ǰ�Сϵͳ�淶�������Դ�Ϊ0 
 * 
 * @param cpld_exist 1:��ѭСϵͳ�淶  0:����ѭСϵͳ�淶
 * 
 * @return 
 */
int cpld_init(int cpld_exist)
{
    m_cpld_init=cpld_exist;
    return m_cpld_init;
    
}

/* �ú�����Ҫϵͳ�齨��ʼ�����ٵ��õģ�����zmDriverInit�����е��� */
void cpld_os_post_init()
{
#ifdef CPLD_INTERRUPT_AUTO_REPEAT
    __cpld_wd = wdCreate();
#endif
}


/* ��ȡPCB major version */
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
 * ���CPLD�Ƿ����
 * @return TRUE(1):����   FALSE(0):δ����
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

/* ��ȡ��ǰbootflashд����״̬
 * return 
 *     1: д����
 *     0: �ر�д����
 */
int bootFlashWPStsGet()
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return !cpld->bootrom_protect_off;
}


/* ���û�ȡbootflashд����״̬
 * ������
 *     enable
 *         1: ��д����
 *         0: �ر�д����
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


/* ���û�ȡ����flash(������CS1��)д����״̬
 * ������
 *     enable
 *         1: ��д����
 *         0: �ر�д����
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






/* ����LED����
 * parameters:
 *      id:LED index
 *      on: 1(led��)    0(LED��)
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
 * Ӳ��λCPU
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



/** ��ȡCPU ID */
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
 *                               ���Ź���������                                     *
 *********************************************************************************/
/* ���Ź�������ÿһ��tick��ʱ����Ĭ�϶���Ϊ40ms */
#define CPLD_WATCHDOG_TICK_LEN 40

/* ��ȡ��ǰ���Ź�ģʽ
 * return ���@ref WATCHDOG_MODE_T
 */
WATCHDOG_MODE_T watchDogStsGet()
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    return (WATCHDOG_MODE_T)cpld->wd_rst_enable;
}





/** 
 * д�뿴�Ź��Ĵ���
 * 
 * @param tickNum ��ʱ��tick��
 */
void WatchDog_regWrite(UINT32 tickNum)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
	
    if(tickNum > 255)
        tickNum = 255;
    cpld->wd_timeout = tickNum;    /* watchdog time out CPLD_WATCHDOG_TICK_LEN * tickNum */
}



/* �رտ��Ź����ر��жϣ������Ź�ģʽǿ������Ϊ�������ж�ģʽ��
 * ����:��
 * ����ֵ:��
 */
void WatchDog_Disable()
{
	
	volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
        /* �ϰ�Сϵͳ��wwd_rst_enable�ǿ��Ź���ʹ��λ */
    cpld->wd_rst_enable = WATCHDOG_DISABLE_RESET;
}



/** 
 * ʹ�ܿ��Ź�
 * �ҽ�WatchDog_Init�д���Ļص����������жϣ������ÿ��Ź�ģʽΪWatchDog_Init�����õ�ģʽ��
 */
void WatchDog_Enable(void)
{
	
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
        /* �򿪿��Ź�ǰ��ιһ�ι� */
    WatchDog_Feed();
    cpld->wd_rst_enable = WATCHDOG_ENABLE_RESET;
        /* �򿪿��Ź�����ιһ�ι� */
    WatchDog_Feed();
}



/** 
 * ι��������ͬ@WatchDog_TimeOut
 * 
 * @param time_ms ��ʱ��ʱ�䣬��λΪms
 */
void WatchDog_Load(UINT32 time_ms)
{
    if(time_ms < CPLD_WATCHDOG_TICK_LEN)
        time_ms = CPLD_WATCHDOG_TICK_LEN;
    WatchDog_regWrite(time_ms/CPLD_WATCHDOG_TICK_LEN - 1);
}

/** 
 * ι��������ͬ@WatchDog_Load
 * @param time_ms ��ʱ��ʱ�䣬��λΪms
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
 * �����¶ȱ���ģ�鹤��ģʽ
 * @param mode ����ģʽ�����@ref TEMP_PROT_MODE
 * @return 
 */
int temperatureProtectModeSet(TEMP_PROT_MODE mode)
{
    volatile struct cpld_rmap * cpld = (volatile struct cpld_rmap *)CPLD_REG_BASE;
    if(mode == TEMP_PROT_INT_ONLY){
        cpld->temperature.w.mode = 0; /* �������ж� */
    }else{
        cpld->temperature.w.mode = 1; /* ������λ����λ */
    }
    return OK;
}



/** 
 * ι��������Ĭ�ϳ�ʱ��WatchDog_Init�����ʱ��ι��
 */
void WatchDog_Feed()
{
    WatchDog_regWrite(watchDogDefaultLoadVal);
}


/** 
 * ���ÿ��Ź��Ĺ���ģʽ
 * @param mode ���@ref WATCHDOG_MODE_T
 * @return OK(0):�ɹ� ERROR(-1):ʧ��
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
 * ��ʼ�����Ź�
 * @param mode ���Ź�����ģʽ�����@ref WATCHDOG_MODE_T
 * @param defaultTimeOutMs Ĭ�ϳ�ʱʱ�䣬��λms
 * @param wdCb �жϴ����Ļص�
 * @param arg �����Ļص��������ݵĲ���
 * 
 * @return OK(0):�ɹ�  ERROR(-1):ʧ��
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
