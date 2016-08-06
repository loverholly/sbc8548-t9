/**
 * @file   cpldDrv.h
 * @author  liujie@LIUJIE
 * @date   Mon Apr 21 16:06:35 2014
 * 
 * @brief  CPLD接口定义文件
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
 * 看门狗工作模式
 */
typedef enum{
    WATCHDOG_DISABLE_RESET = 0, /**< 关闭看门狗 */
    WATCHDOG_ENABLE_RESET /**< 使能看门狗 */
}WATCHDOG_MODE_T;

/** 初始化CPLD */
int cpld_init(int cpld_exist);

int cpld_detect();

/** 获取PCB major version */
int pcb_ver_major();

/** 获取PCB minor version */
int pcb_ver_minor();


/** 获取CPLD major version */
int cpld_ver_major();

/** 获取CPLD minor version */
int cpld_ver_minor();


/** 获取CPU ID */
int cpld_cpu_id();




/** 获取由JP1控制的调试模式状态
 * 返回
 *    0:正常工作模式
 *    1:调试模式
 *    <0:错误
 */
int JPDebugModeGet();


/** 获取由JP2控制的片选交换状态
 * return
 *    0:正常工作模式
 *    1:CS0/CS1片选交换
 */
CS0CS1_STS_T JPCS0CS1SwitchGet();



/** 获取当前bootflash写保护状态
 * return 
 *     1: 写保护
 *     0: 关闭写保护
 */
int bootFlashWPStsGet();


/** 设置获取bootflash写保护状态
 * 参数：
 *     enable
 *         1: 打开写保护
 *         0: 关闭写保护
 */
void bootFlashWPSet(int enable);

/** 设置获取数据flash(连接在CS1上)写保护状态
 * 参数：
 *     enable
 *         1: 打开写保护
 *         0: 关闭写保护
 */
void mainFlashWPSet(int enable);


/** 调试LED控制
 * parameters:
 *      id:LED index
 *      on: 1(led亮)    0(LED灭)
 */
void DbgLEDCtrl(int id, int on);







/** 写cpld的寄存器
 * parameters:
 *       offset:偏移量
 *       bitWidth:位宽  1:8bit 2:16bit 4:32bit
 *       value:值
 */
void cpldWrite(int offset, int bitWidth, unsigned int value);


/** 读取cpld的某个寄存器
 * parameters:
 *       offset:偏移量
 *       bitWidth:位宽  1:8bit 2:16bit 4:32bit
 *       value:值
 */
unsigned int cpldRead(int offset, int bitWidth);


/** 
 * 获取本次启动的类型
 * @return 本次启动的类型,详见BOARD_BOOT_TYPE
 */
int cpuGetBootType();

/* void cpuFreqRegSet(unsigned char val); */
/* unsigned char cpuFreqRegGet(); */

/**
 * 硬复位CPU
 */
void cpuHardReset();



/** 
 * 初始化看门狗
 * @param mode 看门狗工作模式，详见@ref WATCHDOG_MODE_T
 * @param defaultTimeOutMs 默认超时时间，单位ms
 * @param wdCb 中断触发的回调
 * @param arg 触发的回调函数传递的参数
 * 
 * @return OK(0):成功  ERROR(-1):失败
 */
int WatchDog_Init(WATCHDOG_MODE_T mode, int defaultTimeOutMs, FUNCPTR wdCb, UINT32 arg);

/** 关闭看门狗，关闭中断，将看门狗模式强制设置为仅触发中断模式。
 * 参数:无
 * 返回值:无
 */

void WatchDog_Disable();

/** 
 * 使能看门狗
 * 挂接WatchDog_Init中传入的回调函数，打开中断，并设置看门狗模式为WatchDog_Init中设置的模式。
 */
void WatchDog_Enable(void);


/** 
 * 喂狗，功能同@WatchDog_TimeOut
 * 
 * @param time_ms 超时的时间，单位为ms
 */
void WatchDog_Load(UINT32 time_ms);



/** 
 * 喂狗，功能同@WatchDog_Load
 * @param time_ms 超时的时间，单位为ms
 */
void WatchDog_TimeOut(UINT32 time_ms);


/** 
 * 喂狗，按照默认超时或WatchDog_Init传入的时间喂狗
 */
void WatchDog_Feed();

/** 获取当前看门狗模式
 * return 详见@ref WATCHDOG_MODE_T
 */
WATCHDOG_MODE_T watchDogStsGet();

/** 
 * 写入看门狗寄存器
 * 
 * @param tickNum 超时的tick数
 */
void WatchDog_regWrite(UINT32 tickNum);

typedef enum {
    TEMP_PROT_INT_ONLY = 0,     /**< 温度超限时仅发出中断 */
    TEMP_PROT_INT_RST,          /**< 温度超限时发出中断后间隔0.5s复位 */
}TEMP_PROT_MODE;


/** 
 * 获取当前温度保护模块工作模式
 * @return 当前温度保护模式, 详见@ref TEMP_PROT_MODE
 */
TEMP_PROT_MODE temperatureProtectModeGet();

/** 
 * 使能温度保护并挂接中断服务程序
 * @param reset 告警温度
 * @param resume 恢复温度
 * @param cb 发生温度保护时的温度回调
 * @param cbArg 回调函数参数
 * @param mode 工作模式，详见@ref TEMP_PROT_MODE
 */
void temperatureProtectEnable(int reset, int resume, FUNCPTR cb, UINT32 cbArg, TEMP_PROT_MODE mode);


/** 
 * 设置温度保护模块工作模式
 * @param mode 工作模式，详见@ref TEMP_PROT_MODE
 * @return 
 */
int temperatureProtectModeSet(TEMP_PROT_MODE mode);

#endif

