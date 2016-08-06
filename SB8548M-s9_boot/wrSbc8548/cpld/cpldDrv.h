/**
 * @file   cpldDrv.h
 * @author  liujie@LIUJIE
 * @date   Mon Apr 21 16:06:35 2014
 * 
 * @brief  CPLD�ӿڶ����ļ�
 * 
 * 
 */

#ifndef __STD_CPLD_DRV_H__
#define __STD_CPLD_DRV_H__
#include <vxWorks.h>
typedef enum{
    CS0CS1_NORMAL = 0,
    CS0CS1_SWITCH
}CS0CS1_STS_T;

/** 
 * ���Ź�����ģʽ
 */
typedef enum{
    WATCHDOG_DISABLE_RESET = 0, /**< �رտ��Ź� */
    WATCHDOG_ENABLE_RESET /**< ʹ�ܿ��Ź� */
}WATCHDOG_MODE_T;

/** ��ʼ��CPLD */
int cpld_init(int cpld_exist);

int cpld_detect();

/** ��ȡPCB major version */
int pcb_ver_major();

/** ��ȡPCB minor version */
int pcb_ver_minor();


/** ��ȡCPLD major version */
int cpld_ver_major();

/** ��ȡCPLD minor version */
int cpld_ver_minor();


/** ��ȡCPU ID */
int cpld_cpu_id();




/** ��ȡ��JP1���Ƶĵ���ģʽ״̬
 * ����
 *    0:��������ģʽ
 *    1:����ģʽ
 *    <0:����
 */
int JPDebugModeGet();


/** ��ȡ��JP2���Ƶ�Ƭѡ����״̬
 * return
 *    0:��������ģʽ
 *    1:CS0/CS1Ƭѡ����
 */
CS0CS1_STS_T JPCS0CS1SwitchGet();



/** ��ȡ��ǰbootflashд����״̬
 * return 
 *     1: д����
 *     0: �ر�д����
 */
int bootFlashWPStsGet();


/** ���û�ȡbootflashд����״̬
 * ������
 *     enable
 *         1: ��д����
 *         0: �ر�д����
 */
void bootFlashWPSet(int enable);

/** ���û�ȡ����flash(������CS1��)д����״̬
 * ������
 *     enable
 *         1: ��д����
 *         0: �ر�д����
 */
void mainFlashWPSet(int enable);


/** ����LED����
 * parameters:
 *      id:LED index
 *      on: 1(led��)    0(LED��)
 */
void DbgLEDCtrl(int id, int on);







/** дcpld�ļĴ���
 * parameters:
 *       offset:ƫ����
 *       bitWidth:λ��  1:8bit 2:16bit 4:32bit
 *       value:ֵ
 */
void cpldWrite(int offset, int bitWidth, unsigned int value);


/** ��ȡcpld��ĳ���Ĵ���
 * parameters:
 *       offset:ƫ����
 *       bitWidth:λ��  1:8bit 2:16bit 4:32bit
 *       value:ֵ
 */
unsigned int cpldRead(int offset, int bitWidth);


/** 
 * ��ȡ��������������
 * @return ��������������,���BOARD_BOOT_TYPE
 */
int cpuGetBootType();

/* void cpuFreqRegSet(unsigned char val); */
/* unsigned char cpuFreqRegGet(); */

/**
 * Ӳ��λCPU
 */
void cpuHardReset();



/** 
 * ��ʼ�����Ź�
 * @param mode ���Ź�����ģʽ�����@ref WATCHDOG_MODE_T
 * @param defaultTimeOutMs Ĭ�ϳ�ʱʱ�䣬��λms
 * @param wdCb �жϴ����Ļص�
 * @param arg �����Ļص��������ݵĲ���
 * 
 * @return OK(0):�ɹ�  ERROR(-1):ʧ��
 */
int WatchDog_Init(WATCHDOG_MODE_T mode, int defaultTimeOutMs, FUNCPTR wdCb, UINT32 arg);

/** �رտ��Ź����ر��жϣ������Ź�ģʽǿ������Ϊ�������ж�ģʽ��
 * ����:��
 * ����ֵ:��
 */

void WatchDog_Disable();

/** 
 * ʹ�ܿ��Ź�
 * �ҽ�WatchDog_Init�д���Ļص����������жϣ������ÿ��Ź�ģʽΪWatchDog_Init�����õ�ģʽ��
 */
void WatchDog_Enable(void);


/** 
 * ι��������ͬ@WatchDog_TimeOut
 * 
 * @param time_ms ��ʱ��ʱ�䣬��λΪms
 */
void WatchDog_Load(UINT32 time_ms);



/** 
 * ι��������ͬ@WatchDog_Load
 * @param time_ms ��ʱ��ʱ�䣬��λΪms
 */
void WatchDog_TimeOut(UINT32 time_ms);


/** 
 * ι��������Ĭ�ϳ�ʱ��WatchDog_Init�����ʱ��ι��
 */
void WatchDog_Feed();

/** ��ȡ��ǰ���Ź�ģʽ
 * return ���@ref WATCHDOG_MODE_T
 */
WATCHDOG_MODE_T watchDogStsGet();

/** 
 * д�뿴�Ź��Ĵ���
 * 
 * @param tickNum ��ʱ��tick��
 */
void WatchDog_regWrite(UINT32 tickNum);

typedef enum {
    TEMP_PROT_INT_ONLY = 0,     /**< �¶ȳ���ʱ�������ж� */
    TEMP_PROT_INT_RST,          /**< �¶ȳ���ʱ�����жϺ���0.5s��λ */
}TEMP_PROT_MODE;


/** 
 * ��ȡ��ǰ�¶ȱ���ģ�鹤��ģʽ
 * @return ��ǰ�¶ȱ���ģʽ, ���@ref TEMP_PROT_MODE
 */
TEMP_PROT_MODE temperatureProtectModeGet();

/** 
 * ʹ���¶ȱ������ҽ��жϷ������
 * @param reset �澯�¶�
 * @param resume �ָ��¶�
 * @param cb �����¶ȱ���ʱ���¶Ȼص�
 * @param cbArg �ص���������
 * @param mode ����ģʽ�����@ref TEMP_PROT_MODE
 */
void temperatureProtectEnable(int reset, int resume, FUNCPTR cb, UINT32 cbArg, TEMP_PROT_MODE mode);


/** 
 * �����¶ȱ���ģ�鹤��ģʽ
 * @param mode ����ģʽ�����@ref TEMP_PROT_MODE
 * @return 
 */
int temperatureProtectModeSet(TEMP_PROT_MODE mode);

#endif

