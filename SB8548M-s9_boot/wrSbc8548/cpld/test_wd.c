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

/*ι�������ȡֵ*/
#define WD_FEED_MS_NUM                   5  
/*���Ź���FPGA���ƼĴ�����ַ*/
#define WD_FPGA_CONFIG_ADDR              0xf0100010
/*���Ź��Ķ�ʱʱ��check����λ��ms*/
#define WD_CHECK_MS                      (10)
/*���Ź���λ���Եİ�ͷ�����ֶ�*/
#define WD_STRING_FLAG                   "TEST_RESET:"
/*���Ź���λ����ʹ�õ�level����*/
#define WD_RESET_LOG_LEVEL               7
/*���Թ����в��ҵ�ͬһLevel��log�������Ŀ*/
#define WD_RESET_LOG_MAX_NUM             0x1fff
/*���Ź���klog��������ֶε�����*/
#define WD_LOG_FLAG_NUM                  5



IMPORT void	vxTimeBaseGet (UINT32 * pTbu, UINT32 * pTbl);

/*���Ź����뵽�ж���Ĳ�������*/
typedef struct _testWdPara
{
    UINT32 unMode;
    UINT32 unSetMs;
    UINT32 unFeedMs;
    UINT32 unStartTimeBase;
    UINT32 unTestMode;
}TEST_WD_PARA;
/*�������ͣ���Ҫ���Ժͳ�ֲ���*/
typedef enum _testWdMode
{
    WD_NEED_TEST=0,
    WD_FILL_TEST
}TEST_WD_MODE;


/*���Ź��������ڴ��ݵ�callback�Ĳ����ṹ��*/
static TEST_WD_PARA g_stWdPara;
/*���Ź����Խ���Ļ��ܣ��ֱ��¼NEED��FILL���ֲ��Է�ʽ*/
static int g_nRtn[2]={0};
/*���Ź���λģʽ�£�����klog�ĸ�λ������Ϣ*/
static char g_cLogString[59]={0};
/*���Ź���λ���Ե���klog��¼��Ϣ���ݵ��ź���*/
static SEM_ID g_semLog=NULL;              

/** 
 * ��ȡ��������������
 * @return ��������
 */
extern BOARD_BOOT_TYPE bootType();

/** 
 * ��ȡ�忨������������
 * ��API�����������л�����һ�Σ���ʱ��flash������δ��ʼ������˶�ȡflashʹ��ֱ�ӵ�ַ��ȡ�ķ�ʽ
 * @return ��������
 */
extern UINT32 getBoardBootCount();

/** 
 * ��ȡ�����������͵��ַ�������
 * @return �����������͵��ַ�������
 */
extern char * bootTypeString();

/** 
 * ��ȡĳ�����������ͣ���api���û�API����ȡflashʹ�������ӿڣ���˽�ֹ����������ʹ�ø�API
 * @param index 
 * @return 
 */
extern BOARD_BOOT_TYPE getBootType(UINT32 index);

/* ��ȡ��ǰ���Ź�״̬
 * return 
 *     1: ����ж�0.5s��λ
 *     0: �������λ
 */
extern WATCHDOG_MODE_T watchDogStsGet(void);

/** 
 * ι��������Ĭ�ϳ�ʱ��WatchDog_Init�����ʱ��ι��
 */
extern void WatchDog_Feed();

/** 
 * ��ʼ�����Ź�
 * @param mode ���Ź�����ģʽ�����@ref WATCHDOG_MODE_T
 * @param defaultTimeOutMs Ĭ�ϳ�ʱʱ�䣬��λms
 * @param wdCb �жϴ����Ļص�
 * @param arg �����Ļص��������ݵĲ���
 * 
 * @return OK(0):�ɹ�  ERROR(-1):ʧ��
 */
extern int WatchDog_Init(WATCHDOG_MODE_T mode, int defaultTimeOutMs,
    FUNCPTR wdCb, UINT32 arg);


/* �رտ��Ź�
 * ����:��
 * ����ֵ:��
 */
extern void WatchDog_Disable(void);

/* �򿪿��Ź�
 * ����:��
 * ����ֵ:��
 */
extern void WatchDog_Enable(void); 

/** 
 * ι��
 * @param time ��ʱʱ�䣬��λΪ10ms
 */
extern void WatchDog_Load(UINT32 time);

/** 
 * ι��
 * @param time ��ʱʱ�䣬��λΪ10ms
 */
extern void WatchDog_TimeOut(UINT32 time);

/** 
 * ���Ź���λ�ļ�麯��
 * @return ��ȡһ��32λ�������
 */
unsigned int random()
{
    unsigned int unRtn=0;

    unRtn = (rand()<<16) + rand();

    return(unRtn);
}

/** 
 * ����ʱ��һЩ�ؼ������ĳ�ʼ��������:���������
 * @return ��
 */
void testParaInit()
{
    unsigned int unRandH=0;
    unsigned int unRandL=0;

    /*ͨ��vxTimeBaseGet�õ��������*/
    vxTimeBaseGet(&unRandH, &unRandL);
    srand(unRandL);    

    return;
}

/** 
 * 
 * ����basetime���о��ܵĵȴ�
 * @param nDelayMs -- �ȴ���ʱ�䣬��λ�Ǻ���
 * @return ��
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
 * ���Ź���λ���ԵĽ���log�ؼ��ֵĺ�����ֻ�ܽ���"<???> <???> <???>"�ĸ�ʽ
 * @param cString -- log��buf���������ݣ�ȫ��Ϊ64�ֽ�
 * @return 0--���ͨ������0--���δͨ��
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
            /*��¼0x3c���string���׵�ַ*/
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
    /*����ַ����Ƿ񰴹̶���ʽ��������*/
    if(nStartPosIdx != nStopPosIdx)
    {
        printf("\nError : \"<\"��\">\"��������һ��(%d : %d)��",
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
 * ���Ź���λ���Ե�check����
 * @return ��
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
        printf("\nû�в鵽���Ź��ĸ�λ���Լ�¼��Ϣ��");
        return;
    }
    else if(nLogNum > WD_RESET_LOG_MAX_NUM)
    {
        printf("\nƥ���������־������(%d)�����˲��Ի��������(%d)��", nLogNum,
            WD_RESET_LOG_MAX_NUM);
        return;
    }
    /*�������һ��klog��¼*/
    nRtn = testWdLogStringToInt(pcLogs[nLogNum-1], nLogParas);
    if(WD_LOG_FLAG_NUM != nRtn)/*���Ź���λ������ʽΪWD_LOG_FLAG_NUM��<???>�ֶ�*/
    {
        printf("\nKlog���������Ŀ��Ź��ؼ��ֲ�����%d, nRtn=%d��", 
            WD_LOG_FLAG_NUM, nRtn);
    }
    /*���Ź���λ��klog�����Ĺؼ��ֵļ��,��testWdCallback��klogString�е�˳�򱣳�һ��*/
    nWdRecordIdx = nLogParas[0];
    nWdMode = nLogParas[1];
    nTestMode = nLogParas[2];
    nDelayMs = nLogParas[3];
    nRtnMs = nLogParas[4];

    nRtn = getBoardBootCount();
    if(nWdRecordIdx != nRtn-1)
    {
        printf("\n���Ź���λ�������δͨ�� : %d->%d", nWdRecordIdx, nRtn);
        nErr++;
    }
    
    if(nWdMode != WATCHDOG_INTERRUPT_AUTO_RESET)
    {
        printf("\n���Ź���λ��ģʽ���ԣ�nMode=%d.", nWdMode);
        nErr++;
    }

    if((nRtnMs>(nDelayMs+WD_CHECK_MS)) || (nRtnMs<(nDelayMs-WD_CHECK_MS))) 
    {
        printf("\n���Ź���λģʽ�¸�λʱ��������nRtnMs=%d nDelayMs=%d nCheckMs=%d",
            nRtnMs, nDelayMs, WD_CHECK_MS);
        nErr++;
    }

    if(0 == nErr)
    {
        printf("\n���Ź���λ����,PASS.");
    }
    else
    {
        printf("\n���Ź���λ����,FAIL.");
    }

    return;
}

/** 
 * 
 * ���Ź���klog���¼������Ϣ�ĺ�������Ҫ�ͷ��ź��������ִ��
 * @param nLogLevel--��¼����Ϣ��¼��klog��level
 * @param pcLogString--��¼��klog�����Ϣ
 * @return ��
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
 * ���Ź��Զ������Ե�check����
 * @param eMode--���Ź�ģʽ:0Ϊֻ�����жϣ�1Ϊ�����ж�0.5s��λϵͳ
 * @param unSetMs--���õĿ��Ź���λ���ʱ�䣬��λΪms
 * @param unRtnMs--���õĿ��Ź���λ���ʱ�䣬��λΪms
 * @return 0--����ͨ��������0--����ʧ��
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
 * ���Ź��Զ������Ե�check����
 * @param eMode--���Ź�ģʽ:0Ϊֻ�����жϣ�1Ϊ�����ж�0.5s��λϵͳ
 * @param unSetMs--���õĿ��Ź���λ���ʱ�䣬��λΪms
 * @return 0--����ͨ��������0--����ʧ��
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
    /*�����Խ����¼��log��ȥ��Ȼ���´θ�λ���log�ж�ȡ�ؼ����ݵõ����Խ��*/
    if(WATCHDOG_INTERRUPT_AUTO_RESET == unMode)
    {
        /* ��Ҫ��¼mode��wdSetMs��unRtn������������Ź��Ķ�ʱʱ���Ƿ�׼ȷ�� */
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
        logMsg("\ntestWdCallback : Mode=%dΪ�Ƿ�ֵ��������ֹ��", unMode,0,1,2,3,4);
    }

    return;
}

/** 
 * 
 * ���Ź��Զ������Եı�Ҫ�����Ĳ��Ժ���
 * @return ��
*/
int testWdNeedFunc(TEST_WD_PARA* pstWdPara)
{
    int nRtn=0;

    /*��ʼ�����Ի���*/
    WatchDog_Disable();
    /*�ҽӿ��Ź����жϺ���*/
    nRtn = WatchDog_Init(pstWdPara->unMode, pstWdPara->unSetMs,
        (FUNCPTR)testWdCallback, (int)pstWdPara);
    /*��ȡ���Ź�ʹ��ʱ��ʱ�̵�timebase*/
    pstWdPara->unStartTimeBase = cpu_timebase_get();
    if(OK != nRtn)
    {
        printf("\n%s : mode=%d unSetMs=%dʱ�����Ź���ʼ��ʧ�ܡ�",
            __FUNCTION__, pstWdPara->unMode, pstWdPara->unSetMs);
        return(-1);
    }
    
    return(0);
}

/** 
 * 
 * ���Ź��Զ������Եĳ�������Ĳ��Ժ���
 * @return ��
 */
int testWdFillFunc(TEST_WD_PARA* pstWdPara)
{
    int i=0;
    int nRtn=0;
    int nFeedLoop=1;
    int nFeedMs=(pstWdPara->unSetMs)/2;

    /*��ʼ�����Ի���*/
    WatchDog_Disable();
    /*�ҽӿ��Ź����жϺ���*/
    pstWdPara->unFeedMs = nFeedMs*nFeedLoop;
    nRtn = WatchDog_Init(pstWdPara->unMode, pstWdPara->unSetMs,
        (FUNCPTR)testWdCallback, (int)pstWdPara);
    /*��ȡ���Ź�ʹ��ʱ��ʱ�̵�timebase*/
    pstWdPara->unStartTimeBase = cpu_timebase_get();
    if(OK != nRtn)
    {
        printf("\n%s : mode=%d unSetMs=%dʱ�����Ź���ʼ��ʧ�ܡ�",
            __FUNCTION__, pstWdPara->unMode, pstWdPara->unSetMs);
        return(-1);
    }
    /*ι������ʱ��*/
    for(i=0; i<nFeedLoop; i++)
    {
        sysMsDelay(nFeedMs);
        WatchDog_Feed();
    }
   
    return(0);    
}

/** 
 * 
 * ���Ź��Զ������Ե�������
 * @param eWdMode -- ���Ź���ģʽ��0Ϊֻ�����жϣ�1Ϊ�����жϺ�λ
 * @param eTestMode -- ���Ź��Ĳ���ģʽ��0Ϊ��Ҫ�������ԣ�1Ϊ�����������
 * @param unSetMs -- ���Ź���ʱ��ʱ�䳤�ȣ���λΪms
 * @return -1--mode�����ˣ�0--����ͨ��������0--����ʧ��
 */
int testWdAutoFunc(WATCHDOG_MODE_T eWdMode, TEST_WD_MODE eTestMode, UINT32 unSetMs)
{
    int nErr=0;

    if((eWdMode!=WATCHDOG_INTERRUPT_AUTO_RESET)&&(eWdMode!=WATCHDOG_ONLY_INTERRUPT))
    {
        printf("\n%s : WdMode=%dΪ�Ƿ�ֵ��������ֹ��", __FUNCTION__, eWdMode);
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
        /*�ȴ����Ź����ж����ͷ��ź�����Ȼ�󽫸�λ��Ϣ��¼��klog��ȥ*/    
        testWdLogRecord(g_cLogString);
    }
    sysMsDelay(unSetMs+10);
    nErr=g_nRtn[eTestMode];

    return(nErr);
}

/** 
 * 
 * ���Ź��Զ������Ե�������
 * @return -1--mode�����ˣ�0--����ͨ��������0--����ʧ��
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
            printf("\nONLY_INT[%d] : ����ֵ[", nTestMode);
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
                printf("]���ԣ�PASS.");
            }
            else
            {
                printf("]���ԣ�FAIL.");
            }
        }
    }

    return;
}

#ifdef INCLUDE_ZMD_SW_KDM
/** 
 * Ϊ��ǰ���Ź�����KDM�ĸ���ڵ㣬����һ�������õ�KDM��
 * @return  ��
 */
void testWdKdmInit(void * pKdmFatherNode)
{
    char pcNodeName[50]={0};
    void * pHwtKdmRoot=NULL;
    void * pHwtKdmFstNode=NULL;

    /*����HWT��KDM�ĸ��ڵ�*/
    sprintf(pcNodeName, "%s", "���Ź�����");
    pHwtKdmRoot = KDM_ADD(pKdmFatherNode, pcNodeName, NULL, 0);
    if(NULL == pHwtKdmRoot)
    {
        PERROR("%s add failed", pcNodeName);
        return;
    }
    /*����KDM�б��Ӳ����ʱ�����б�*/
    sprintf(pcNodeName, "%s", "���Ź��Զ����Բ˵�");
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

