#ifdef INCLUDE_ZMD_SW_KDM
#include "vxWorks.h"
#include <stdio.h>
#include <string.h>
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "fioLib.h"
#include <time.h>
#include <stdlib.h>
#include "arch/generic/klib.h"
#include <drivers/cpld/cpldDrv.h>
#include "libs/debug.h"
#include "libs/kdm/kdm.h"
#include "libs/klog/klog.h"
#include "libs/klog/klog_ll.h"
#include "zmdArchLib.h"

/*喂狗的最大取值*/
#define WD_FEED_MS_NUM                   5  
/*看门狗的FPGA控制寄存器地址*/
#define WD_FPGA_CONFIG_ADDR              0xf0100010
/*看门狗的定时时间check误差，单位是ms*/
#define WD_CHECK_MS                      (10)
/*看门狗复位测试的包头敏感字段*/
#define WD_STRING_FLAG                   "TEST_RESET:"
/*看门狗复位测试使用的level级别*/
#define WD_RESET_LOG_LEVEL               7
/*测试过程中查找的同一Level的log的最大数目*/
#define WD_RESET_LOG_MAX_NUM             0x1fff
/*看门狗的klog里的敏感字段的数量*/
#define WD_LOG_FLAG_NUM                  5



IMPORT void	vxTimeBaseGet (UINT32 * pTbu, UINT32 * pTbl);

/*看门狗传入到中断里的参数类型*/
typedef struct _testWdPara
{
    UINT32 unMode;
    UINT32 unSetMs;
    UINT32 unFeedMs;
    UINT32 unStartTimeBase;
    UINT32 unTestMode;
}TEST_WD_PARA;
/*测试类型，必要测试和充分测试*/
typedef enum _testWdMode
{
    WD_NEED_TEST=0,
    WD_FILL_TEST
}TEST_WD_MODE;


/*看门狗测试用于传递到callback的参数结构体*/
static TEST_WD_PARA g_stWdPara;
/*看门狗测试结果的汇总，分别记录NEED、FILL两种测试方式*/
static int g_nRtn[2]={0};
/*看门狗复位模式下，存入klog的复位测试信息*/
static char g_cLogString[59]={0};
/*看门狗复位测试的向klog记录信息内容的信号量*/
static SEM_ID g_semLog=NULL;              

/** 
 * 获取本次启动的类型
 * @return 启动类型
 */
extern BOARD_BOOT_TYPE bootType();

/** 
 * 获取板卡启动次数计数
 * 该API在启动过程中会运行一次，此时，flash驱动尚未初始化，因此读取flash使用直接地址读取的方式
 * @return 启动次数
 */
extern UINT32 getBoardBootCount();

/** 
 * 获取本次启动类型的字符串描述
 * @return 本次启动类型的字符串描述
 */
extern char * bootTypeString();

/** 
 * 获取某次启动的类型，该api是用户API，读取flash使用驱动接口，因此禁止在启动过程使用该API
 * @param index 
 * @return 
 */
extern BOARD_BOOT_TYPE getBootType(UINT32 index);

/* 获取当前看门狗状态
 * return 
 *     1: 输出中断0.5s后复位
 *     0: 仅输出复位
 */
extern WATCHDOG_MODE_T watchDogStsGet(void);

/** 
 * 喂狗，按照默认超时或WatchDog_Init传入的时间喂狗
 */
extern void WatchDog_Feed();

/** 
 * 初始化看门狗
 * @param mode 看门狗工作模式，详见@ref WATCHDOG_MODE_T
 * @param defaultTimeOutMs 默认超时时间，单位ms
 * @param wdCb 中断触发的回调
 * @param arg 触发的回调函数传递的参数
 * 
 * @return OK(0):成功  ERROR(-1):失败
 */
extern int WatchDog_Init(WATCHDOG_MODE_T mode, int defaultTimeOutMs,
    FUNCPTR wdCb, UINT32 arg);


/* 关闭看门狗
 * 参数:无
 * 返回值:无
 */
extern void WatchDog_Disable(void);

/* 打开看门狗
 * 参数:无
 * 返回值:无
 */
extern void WatchDog_Enable(void); 

/** 
 * 喂狗
 * @param time 超时时间，单位为10ms
 */
extern void WatchDog_Load(UINT32 time);

/** 
 * 喂狗
 * @param time 超时时间，单位为10ms
 */
extern void WatchDog_TimeOut(UINT32 time);

/** 
 * 看门狗复位的检查函数
 * @return 获取一个32位的随机数
 */
unsigned int random()
{
    unsigned int unRtn=0;

    unRtn = (rand()<<16) + rand();

    return(unRtn);
}

/** 
 * 测试时的一些关键参数的初始化，比如:随机数种子
 * @return 无
 */
void testParaInit()
{
    unsigned int unRandH=0;
    unsigned int unRandL=0;

    /*通过vxTimeBaseGet得到随机种子*/
    vxTimeBaseGet(&unRandH, &unRandL);
    srand(unRandL);    

    return;
}

/** 
 * 
 * 利用basetime进行精密的等待
 * @param nDelayMs -- 等待的时间，单位是毫秒
 * @return 无
 */
void testTimeBaseDelay(int nDelayMs)
{
    unsigned int unStartTimeBase=0;

    unStartTimeBase=cpu_timebase_get();
    while(TRUE)
    {
        if(cpu_time_diff_us(unStartTimeBase) >= (unsigned int)nDelayMs*1000)
        {
            break;
        }
    }

    return;
}

/** 
 * 看门狗复位测试的解析log关键字的函数，只能解析"<???> <???> <???>"的格式
 * @param cString -- log的buf的所有内容，全长为64字节
 * @return 0--检查通过；非0--检查未通过
 */
int testWdLogStringToInt(char* cString, int* nLogParas)
{
    int nCnt=0; 
    int nStartPos[10]={0};
    int nStopPos[10]={0};
    int nStartPosIdx=0;
    int nStopPosIdx=0;
    int nIdx=0;
    char cTempString[64]={0};

    for(nCnt=0; nCnt<64-5; nCnt++)
    {
        if('<' == cString[nCnt])
        {
            /*记录0x3c后的string的首地址*/
            nStartPos[nStartPosIdx]=nCnt+1;
            nStartPosIdx++;
        }
    }
    
    for(nCnt=0; nCnt<64-5; nCnt++)
    {
        if('>' == cString[nCnt])
        {
            nStopPos[nStopPosIdx]=nCnt;
            nStopPosIdx++;
        }
    }
    /*检查字符串是否按固定格式来解析的*/
    if(nStartPosIdx != nStopPosIdx)
    {
        printf("\nError : \"<\"与\">\"的数量不一致(%d : %d)。",
            nStartPosIdx, nStopPosIdx);
        return(-1);
    }
    
    for(nIdx=0; nIdx<nStartPosIdx; nIdx++)
    {
        if(nIdx < nStartPosIdx-1)
        {
            if(nStopPos[nIdx]<=nStartPos[nIdx])
            {
                printf("\nError : nStopPos[%d]=%d nStartPos[%d]=%d", nIdx, 
                    nStopPos[nIdx], nIdx, nStartPos[nIdx]);
                return(-1);
            }

            if(nStopPos[nIdx]>=nStartPos[nIdx+1])
            {
                printf("\nError : nStopPos[%d]=%d nStartPos[%d]=%d", nIdx, 
                    nStopPos[nIdx], nIdx+1, nStartPos[nIdx+1]);
                return(-1);
            }
        }
    }

    for(nIdx=0; nIdx<nStartPosIdx; nIdx++)
    {
        memset(cTempString, 0, sizeof(cTempString));
        memcpy(cTempString, &(cString[nStartPos[nIdx]]), nStopPos[nIdx]-nStartPos[nIdx]);
        nLogParas[nIdx] = atoi(cTempString);
   }

    return(nStartPosIdx);
}

/** 
 * 
 * 看门狗复位测试的check函数
 * @return 无
 */
void testWdLogCheck(void)
{
    char ** pcLogs = NULL;
    int nLogParas[10]={0};
    int nWdRecordIdx=0;
    int nWdMode=0;
    int nTestMode=0;
    int nDelayMs=0;
    int nRtnMs=0;
    int nRtn=0;
    int nLogNum=0;
    int nErr=0;

    pcLogs = klogSelect(LOGTYPE_WD, WD_RESET_LOG_LEVEL, WD_STRING_FLAG, &nLogNum);
    if(0 == nLogNum)
    {
        printf("\n没有查到看门狗的复位测试记录信息。");
        return;
    }
    else if(nLogNum > WD_RESET_LOG_MAX_NUM)
    {
        printf("\n匹配的敏感日志的条数(%d)超过了测试缓存的上限(%d)。", nLogNum,
            WD_RESET_LOG_MAX_NUM);
        return;
    }
    /*解析最后一个klog记录*/
    nRtn = testWdLogStringToInt(pcLogs[nLogNum-1], nLogParas);
    if(WD_LOG_FLAG_NUM != nRtn)/*看门狗复位解析格式为WD_LOG_FLAG_NUM个<???>字段*/
    {
        printf("\nKlog解析出来的看门狗关键字不等于%d, nRtn=%d。", 
            WD_LOG_FLAG_NUM, nRtn);
    }
    /*看门狗复位的klog解析的关键字的检查,与testWdCallback中klogString中得顺序保持一致*/
    nWdRecordIdx = nLogParas[0];
    nWdMode = nLogParas[1];
    nTestMode = nLogParas[2];
    nDelayMs = nLogParas[3];
    nRtnMs = nLogParas[4];

    nRtn = getBoardBootCount();
    if(nWdRecordIdx != nRtn-1)
    {
        printf("\n看门狗复位计数检查未通过 : %d->%d", nWdRecordIdx, nRtn);
        nErr++;
    }
    
    if(nWdMode != WATCHDOG_INTERRUPT_AUTO_RESET)
    {
        printf("\n看门狗复位的模式不对，nMode=%d.", nWdMode);
        nErr++;
    }

    if((nRtnMs>(nDelayMs+WD_CHECK_MS)) || (nRtnMs<(nDelayMs-WD_CHECK_MS))) 
    {
        printf("\n看门狗复位模式下复位时间误差过大，nRtnMs=%d nDelayMs=%d nCheckMs=%d",
            nRtnMs, nDelayMs, WD_CHECK_MS);
        nErr++;
    }

    if(0 == nErr)
    {
        printf("\n看门狗复位测试,PASS.");
    }
    else
    {
        printf("\n看门狗复位测试,FAIL.");
    }

    return;
}

/** 
 * 
 * 看门狗向klog里记录测试信息的函数，需要释放信号量后才能执行
 * @param nLogLevel--记录的信息记录到klog的level
 * @param pcLogString--记录到klog里的信息
 * @return 无
 */
static void testWdLogRecord(char* pcLogString)
{
    if (semTake(g_semLog, WAIT_FOREVER) < 0)
    {
        printf("\nsemTake(%d) fail.", (int)g_semLog);
        return ;
    }

    klog(LOGTYPE_WD, WD_RESET_LOG_LEVEL, pcLogString);

    return;
}

/** 
 * 
 * 看门狗自动化测试的check函数
 * @param eMode--看门狗模式:0为只触发中断，1为触发中断0.5s后复位系统
 * @param unSetMs--设置的看门狗复位间隔时间，单位为ms
 * @param unRtnMs--设置的看门狗复位间隔时间，单位为ms
 * @return 0--测试通过；大于0--测试失败
 */
int testWdAutoCheck(WATCHDOG_MODE_T eMode, UINT32 unDelayMs, UINT32 unRtnMs)
{
    int nErr=0;

    if(WATCHDOG_ONLY_INTERRUPT == eMode)
    {
        if((unRtnMs>(unDelayMs+WD_CHECK_MS)) || (unRtnMs<(unDelayMs-WD_CHECK_MS))) 
        {
            nErr++;
        }
    }

    return(nErr);
}

/** 
 * 
 * 看门狗自动化测试的check函数
 * @param eMode--看门狗模式:0为只触发中断，1为触发中断0.5s后复位系统
 * @param unSetMs--设置的看门狗复位间隔时间，单位为ms
 * @return 0--测试通过；大于0--测试失败
 */
void testWdCallback(TEST_WD_PARA* pstWdPara)
{
    int nRtn=0;
    UINT32 unRtnMs=0;
    UINT32 unMode=0;
    UINT32 unDelayMs=0;
    UINT32 unStartTimeBase=0;
    UINT32 unTestMode=0;

    unMode = pstWdPara->unMode;
    unStartTimeBase = pstWdPara->unStartTimeBase;
    unTestMode = pstWdPara->unTestMode;
    unDelayMs = pstWdPara->unSetMs+pstWdPara->unFeedMs;
    
    unRtnMs = cpu_time_diff_us(unStartTimeBase)/1000;
    /*将测试结果记录到log中去，然后下次复位后从log中读取关键数据得到测试结果*/
    if(WATCHDOG_INTERRUPT_AUTO_RESET == unMode)
    {
        /* 需要记录mode、wdSetMs和unRtn，才能算出看门狗的定时时间是否准确。 */
        sprintf(g_cLogString, "%s%s%d%s%d%s%d%s%d%s%d%s", WD_STRING_FLAG,
            "<", getBoardBootCount(), "><", unMode, "><", unTestMode, 
            "><", unDelayMs, "><", unRtnMs, ">");
        semGive(g_semLog);
    }
    else if(WATCHDOG_ONLY_INTERRUPT == unMode)
    {
        nRtn=testWdAutoCheck(unMode, unDelayMs, unRtnMs);
        if(0 == nRtn)
        {
#if TEST_DEBUG        
            logMsg("\nONLY_INT[%d] : unSetMs=%d unRtnMs=%d nCheckMs=%d  PASS.", 
                unTestMode, unDelayMs,unRtnMs,WD_CHECK_MS,2,3,4);
#endif
        }
        else
        {
            g_nRtn[unTestMode]++;
            logMsg("\nONLY_INT[%d] : unSetMs=%d unRtnMs=%d nCheckMs=%d  FAIL.", 
                unTestMode, unDelayMs,unRtnMs,WD_CHECK_MS,2,3);
        }
    }
    else
    {
        logMsg("\ntestWdCallback : Mode=%d为非法值，测试中止。", unMode,0,1,2,3,4);
    }

    return;
}

/** 
 * 
 * 看门狗自动化测试的必要条件的测试函数
 * @return 无
*/
int testWdNeedFunc(TEST_WD_PARA* pstWdPara)
{
    int nRtn=0;

    /*初始化测试环境*/
    WatchDog_Disable();
    /*挂接看门狗的中断函数*/
    nRtn = WatchDog_Init(pstWdPara->unMode, pstWdPara->unSetMs,
        (FUNCPTR)testWdCallback, (int)pstWdPara);
    /*获取看门狗使能时的时刻的timebase*/
    pstWdPara->unStartTimeBase = cpu_timebase_get();
    if(OK != nRtn)
    {
        printf("\n%s : mode=%d unSetMs=%d时，看门狗初始化失败。",
            __FUNCTION__, pstWdPara->unMode, pstWdPara->unSetMs);
        return(-1);
    }
    
    return(0);
}

/** 
 * 
 * 看门狗自动化测试的充分条件的测试函数
 * @return 无
 */
int testWdFillFunc(TEST_WD_PARA* pstWdPara)
{
    int i=0;
    int nRtn=0;
    int nFeedLoop=1;
    int nFeedMs=(pstWdPara->unSetMs)/2;

    /*初始化测试环境*/
    WatchDog_Disable();
    /*挂接看门狗的中断函数*/
    pstWdPara->unFeedMs = nFeedMs*nFeedLoop;
    nRtn = WatchDog_Init(pstWdPara->unMode, pstWdPara->unSetMs,
        (FUNCPTR)testWdCallback, (int)pstWdPara);
    /*获取看门狗使能时的时刻的timebase*/
    pstWdPara->unStartTimeBase = cpu_timebase_get();
    if(OK != nRtn)
    {
        printf("\n%s : mode=%d unSetMs=%d时，看门狗初始化失败。",
            __FUNCTION__, pstWdPara->unMode, pstWdPara->unSetMs);
        return(-1);
    }
    /*喂狗若干时间*/
    for(i=0; i<nFeedLoop; i++)
    {
        sysMsDelay(nFeedMs);
        WatchDog_Feed();
    }
   
    return(0);    
}

/** 
 * 
 * 看门狗自动化测试的主函数
 * @param eWdMode -- 看门狗的模式，0为只触发中断，1为触发中断后复位
 * @param eTestMode -- 看门狗的测试模式，0为必要条件测试，1为充分条件测试
 * @param unSetMs -- 看门狗定时的时间长度，单位为ms
 * @return -1--mode传错了；0--测试通过；大于0--测试失败
 */
int testWdAutoFunc(WATCHDOG_MODE_T eWdMode, TEST_WD_MODE eTestMode, UINT32 unSetMs)
{
    int nErr=0;

    if((eWdMode!=WATCHDOG_INTERRUPT_AUTO_RESET)&&(eWdMode!=WATCHDOG_ONLY_INTERRUPT))
    {
        printf("\n%s : WdMode=%d为非法值，测试中止。", __FUNCTION__, eWdMode);
        return(-1);
    }

    g_stWdPara.unMode = eWdMode;
    g_stWdPara.unSetMs = unSetMs;
    g_stWdPara.unFeedMs = 0;
    g_stWdPara.unTestMode = (UINT32)eTestMode;

    if(WATCHDOG_INTERRUPT_AUTO_RESET == eWdMode)
    {
        g_semLog = semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);
    }

    if(WD_NEED_TEST == eTestMode)
    {
        testWdNeedFunc(&g_stWdPara);
    }
    else
    {
        testWdFillFunc(&g_stWdPara);
    }

    if(WATCHDOG_INTERRUPT_AUTO_RESET == eWdMode)
    {
        /*等待看门狗在中断中释放信号量，然后将复位信息记录到klog中去*/    
        testWdLogRecord(g_cLogString);
    }
    sysMsDelay(unSetMs+10);
    nErr=g_nRtn[eTestMode];

    return(nErr);
}

/** 
 * 
 * 看门狗自动化测试的主函数
 * @return -1--mode传错了；0--测试通过；大于0--测试失败
 */
void testWdAutoMain()
{
    int nErr[2]={0};
    int nWdMode=0;
    int nTestMode=0;
    UINT32 unSetMs[WD_FEED_MS_NUM]={10, 100, 1000, 10000, 25500};
    int i=0;

    if(WATCHDOG_ONLY_INTERRUPT == nWdMode)
    {
        for(i=0; i<WD_FEED_MS_NUM; i++)
        {
            for(nTestMode=0; nTestMode<2; nTestMode++)
            {
                nErr[nTestMode]+=testWdAutoFunc(nWdMode, nTestMode, unSetMs[i]);
            }
        }

        for(nTestMode=0; nTestMode<2; nTestMode++)
        {
            printf("\nONLY_INT[%d] : 典型值[", nTestMode);
            for(i=0; i<WD_FEED_MS_NUM; i++)
            {
                printf("%d", unSetMs[i]);
                if(i != WD_FEED_MS_NUM-1)
                {
                    printf(", ");
                }
            }
            if(0 == nErr[nTestMode])
            {
                printf("]测试，PASS.");
            }
            else
            {
                printf("]测试，FAIL.");
            }
        }
    }

    return;
}

#ifdef INCLUDE_ZMD_SW_KDM
/** 
 * 为当前看门狗创建KDM的各层节点，构建一个测试用的KDM表
 * @return  无
 */
void testWdKdmInit(void * pKdmFatherNode)
{
    char pcNodeName[50]={0};
    void * pHwtKdmRoot=NULL;
    void * pHwtKdmFstNode=NULL;

    /*创建HWT的KDM的根节点*/
    sprintf(pcNodeName, "%s", "看门狗测试");
    pHwtKdmRoot = KDM_ADD(pKdmFatherNode, pcNodeName, NULL, 0);
    if(NULL == pHwtKdmRoot)
    {
        PERROR("%s add failed", pcNodeName);
        return;
    }
    /*创建KDM列表的硬件定时器的列表*/
    sprintf(pcNodeName, "%s", "看门狗自动测试菜单");
    pHwtKdmFstNode = KDM_ADD(pHwtKdmRoot, pcNodeName, testWdAutoMain, 0);
    if(NULL == pHwtKdmFstNode)
    {
        PERROR("%s add failed", pcNodeName);
        return;
    }

    return;
}
#endif
#endif

