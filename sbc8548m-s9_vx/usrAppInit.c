/* usrAppInit.c - stub application initialization routine */

/* Copyright (c) 1998,2006 Wind River Systems, Inc. 
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
  modification history
  --------------------
  01b,16mar06,jmt  Add header file to find USER_APPL_INIT define
  01a,02jun98,ms   written
*/

/*
  DESCRIPTION
  Initialize user application code.
*/ 

#include <vxWorks.h>
#include "config.h"
#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif /* defined PRJ_BUILD */

#ifdef	INCLUDE_TFFS_MOUNT
void usrTffsMount(void)
{
	if(OK!=usrTffsConfig (1, 0, "/tffs1" ))
	{
		printf("挂载数据FLASH不成功.\r\n");
	}
}
#endif	/* INCLUDE_TFFS_MOUNT */

void usrNetDrvInit(void)
{
	int i = 0;
	char netcfg[80];
	
	for (i = 0; i < 4; i++)
	{
		ipcom_drv_eth_init("motetsec",i,0);
		sprintf(netcfg, "motetsec%d up", i);
		ifconfig(netcfg);
	}

}

#include <moduleLib.h>
#include <private/loadFileLibP.h>
#include <private/loadElfLibP.h>
#include <loadLib.h>
#include <symLib.h>
#include <vmLib.h>
#include <elf.h>
#include <ioLib.h>
char *pText = NULL;
char *pData = NULL;
char *pBss = NULL;

STATUS appLoadFromFs(void)
{
    char        *objName = "/tffs0/AppRun.out";
    int         fd = -1;
    MODULE_ID   mod_id;
    pText = memalign(4, 0x400000);
    pData = memalign(4, 0x400000);
    pBss = memalign(4, 0x400000);
	if ((fd = open(objName, O_RDONLY, 0)) == ERROR) {
		perror("no app file\r\n");
		return ERROR;
	}
	pText = pData = pBss = LD_NO_ADDRESS;
	if ((mod_id = loadModuleAt(fd, LOAD_ALL_SYMBOLS/*|LOAD_FORCE*/,&pText,&pData,&pBss)) == NULL) {
		close(fd);
		perror("loadModule error");
		return ERROR;
	}
    if(fd >= 0)
    {
        close(fd);
    }
    return OK;
}

int appTaskRun(char * taskEntryName, char * taskName, int taskPriority, int taskStackSize)
{
    char *pValue;
    SYM_TYPE symType;
    extern SYMTAB_ID sysSymTbl;
    
        /* 查找入口函数 */
    if (symByCNameFind(sysSymTbl, taskEntryName,
                      &pValue, &symType) == ERROR) {
    	printf("找不到入口函数[%s]\r\n",taskEntryName);
    	symShow(sysSymTbl,"AppRun");
        return -1;
    }
    

    if(0 > taskSpawn(taskName, taskPriority, VX_NO_STACK_FILL| VX_FP_TASK | VX_SPE_TASK, 
                     taskStackSize, (FUNCPTR)pValue,   
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0)){
        printf("创建任务 %s 失败\n", taskName);
        return -2;
    }
    return 0;
}


STATUS appRunInit()
{
	if(appLoadFromFs()!=OK)
	{
		printf("加载AppRun失败\n");
		return ERROR;
	}
	if(OK!=appTaskRun("AppRun","AppRun",201,0x400000))
	{
		printf("AppRun运行失败\n");
	}
	
}

void usrNs16550Init(void)
{
	/* CPU uart2 */
    sysSerialChanConnect(1);
    ttyDrv();
    ttyDevCreate ("/Rs232/0", sysSerialChanGet(1), 1024, 1024);

	/* LBC CS2 - UART - 1MB, 8-bit, modifyed by lih, 20140827 */
    sysSerialChanConnect(2);
    ttyDrv();
    ttyDevCreate ("/Rs485/0", sysSerialChanGet(2), 1024, 1024);
	
	/* LBC CS4 - UART - 1MB, 8-bit, modifyed by lih, 20140827 */
    sysSerialChanConnect(3);
    ttyDrv();
    ttyDevCreate ("/Rs485/1", sysSerialChanGet(3), 1024, 1024);
	
	/* LBC CS5 - UART - 1MB, 8-bit, modifyed by lih, 20140827 */
    sysSerialChanConnect(4);
    ttyDrv();
    ttyDevCreate ("/Rs485/2", sysSerialChanGet(4), 1024, 1024);
	
	/* LBC CS6 - UART - 1MB, 8-bit, modifyed by lih, 20140827 */
    sysSerialChanConnect(5);
    ttyDrv();
    ttyDevCreate ("/Rs485/3", sysSerialChanGet(5), 1024, 1024);

}

/******************************************************************************
 *
 * usrAppInit - initialize the users application
 */ 

void usrAppInit (void)
{
#ifdef	USER_APPL_INIT
	USER_APPL_INIT;		/* for backwards compatibility */
#endif

		/* add application specific code here */
	usrNetDrvInit();
	
#ifdef	INCLUDE_TFFS_MOUNT
	usrTffsMount();
#endif
		/* 用户自定义串口驱动 */
	usrNs16550Init();
		/* 添加I2C驱动 */
	fsl_i2c_init();
	
	appRunInit();
}


