#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <iosLib.h>
#include <private/iosLibP.h>
#include <stat.h>
#include <usrConfig.h>
#include <bootApp.h>
#include "bootlib.h"


/* defines */

/* typedefs */

/* globals */
extern STATUS f2xSet
    (
    int   i,        /* group ID */
    char *  pSrc,      /* source buffer */
    int     nbytes,    /* number of bytes to copy */
    int     offset     /* byte offset into group */
	 );
extern void mtdShow();
extern STATUS sysTffsDosFormat(int i);
extern bootShellPrintfPrototype * bootShellPrintfFunc;
extern char *sysBootLine;
/* locals */


/* forward declarations */


/******************************************************************************
*
*
* bootFsLoadInit - Initialize file system load component
*
* Initializes the boot loader application file system load component.
* 
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/


LOCAL int tffsFromat
    (
    void* arg,
    char * cmd  /* input command string */
    )
    {
    int i=(int)arg;
    sysTffsDosFormat (i);
    return BOOT_STATUS_COMPLETE;
    }

LOCAL int testParamGroup
	(
    void* arg,
    char * cmd  /* input command string */
    )
{
	#define PARAM_PART_SIZE 0x20000
	int i = 0;
	char *wbuf = malloc(PARAM_PART_SIZE);
	char *rbuf = malloc(PARAM_PART_SIZE);
	
	for (i = 0; i < PARAM_PART_SIZE; i++)
	{
		wbuf[i] = rand();
	}
	
	memset(rbuf, 0, PARAM_PART_SIZE);
	sysNvRamSet(wbuf, PARAM_PART_SIZE, 0x0);
	sysNvRamGet(rbuf, PARAM_PART_SIZE, 0x0);
	
	if (memcmp(rbuf, wbuf, PARAM_PART_SIZE) == 0)
	{
		printf("test param partition ok!\r\n");
	}
	else
	{
		printf("test param partition failed!\r\n");
	}
	free(wbuf);
	free(rbuf);

	return 0;
}

LOCAL int testEraseGroup
	(
    void* arg,
    char * cmd  /* input command string */
    )
{
#define PARAM_PART_SIZE 0x20000
	char *wbuf = malloc(PARAM_PART_SIZE);
	char *rbuf = malloc(PARAM_PART_SIZE);
	
	memset(wbuf, 0xff,PARAM_PART_SIZE);
	memset(rbuf, 0, PARAM_PART_SIZE);
	sysNvRamSet(wbuf, PARAM_PART_SIZE, 0x0);
	sysNvRamGet(rbuf, PARAM_PART_SIZE, 0x0);
	
	if (memcmp(rbuf, wbuf, PARAM_PART_SIZE) == 0)
	{
		printf("erase param partition ok!\r\n");
	}
	else
	{
		printf("erase param partition failed!\r\n");
	}
	free(wbuf);
	free(rbuf);
	
	return 0;
}

LOCAL int mtdShowAll
    (
    char * cmd  /* input command string */
    )
    {
    mtdShow ();
    return BOOT_STATUS_COMPLETE;
    }

int ftpfiletoram(char* host,char* user,char* passwd,char *srcFile,char*desRamAddr)
{
    int total=0;
    int ctrlSock,dataSock;  
    char buf[1024];
    int nBytes,times;

    if(ftpXfer(host,user,passwd,"","RETR %s","",srcFile,&ctrlSock,&dataSock)==ERROR)
    {
        printf("ftp error\n");
        return (ERROR);
    }
    printf("\n");
    times=0;
    while((nBytes=read(dataSock,buf,512))>0)
    {
        total+=nBytes;
        if(total > (RAM_HIGH_ADRS - RAM_LOW_ADRS))
        {
            printf("too large image file: %s!\n", srcFile);
            return (ERROR);
        }
        memcpy(desRamAddr,buf,nBytes);
        desRamAddr+=nBytes;
        times++;
        if(times==100) /*printf "*" per/50K data*/
        {
            printf("*");
            times=0;
        }
    }
    printf("\n");
    close(dataSock);
    close(ctrlSock);

    return total;
}

STATUS userUpLoadFile(char* srcFile,char** pbuf,int* size)
{
    int ret=ERROR;
    char* ramPtr;
    int datanum;
    BOOT_PARAMS param;
    if(OK!=usrBootLineCrack (sysBootLine, &param))
        {
            bootAppPrintf("启动参数解析错误\r\n",1,2,3,4,5,6);
            goto err;
        }
  
    ramPtr= (char*)(RAM_LOW_ADRS + 4);
    datanum=ftpfiletoram(param.had,param.usr,param.passwd,srcFile,ramPtr);
    if(datanum!=ERROR)
        {
            if(size) *size=datanum;
            if(pbuf) *pbuf=ramPtr;
            bootAppPrintf("transer done... file[%s] len[0x%x]\r\n",srcFile,(int)*size,3,4,5,6);
            ret=OK;
        }
    err:
    return (ret);
}

LOCAL int ubootrom
    (
    void* arg,
    char * cmd  /* input command string */
    )
    {
    char* pbuf=0;
    int   len=0;
        if(userUpLoadFile("bootrom.bin",&pbuf,&len)==OK)
            {
                bootAppPrintf("write to flash......",1,2,3,4,5,6);
                f2xSet(0,pbuf,len,0);
                bootAppPrintf("done\r\n",1,2,3,4,5,6);
            }
        
        return BOOT_STATUS_COMPLETE;
    }
#if 0
LOCAL int uvxworks
    (
    void* arg;
    char * cmd  /* input command string */
    )
    {
    char* pbuf=0;
    int   len=0;
        if(userUpLoadFile("vxworks.bin",&pbuf,&len)==OK)
            {
                f2xSet(1,&len,4,0);
                f2xSet(1,pbuf,len,4);
            }
        
        return BOOT_STATUS_COMPLETE;
    }
LOCAL int uapprun
    (
    void* arg;
    char * cmd  /* input command string */
    )
    {
    char* pbuf=0;
    int   len=0;
        if(userUpLoadFile("apprun.out",&pbuf,&len)==OK)
            {
                f2xSet(2,&len,4,0);
                f2xSet(2,pbuf,len,4);
            }
        
        return BOOT_STATUS_COMPLETE;
    }
#else
LOCAL int uvxworks
    (
    void* arg,
    char * cmd  /* input command string */
    )
    {
    #if 0
        rm("/tffs0/vxworks");
        cp("vxworks","/tffs0/vxworks");
        return BOOT_STATUS_COMPLETE;
        #else
        char* pbuf=0;
        int   len=0;
        if(userUpLoadFile("vxworks",&pbuf,&len)==OK)
            {
                rm("/tffs0/vxworks");
                int fd=open("/tffs0/vxworks",0x202,777);
                if(fd!=ERROR)
                    {
                       int actLen;
                       bootAppPrintf("write to flash......",1,2,3,4,5,6);
                       actLen=write(fd,pbuf,len);
                       if(actLen==len)
                        {
                            close(fd);
                            bootAppPrintf("done\r\n",1,2,3,4,5,6);
                        }
                       else
                        {
                             bootAppPrintf("write file err.\r\n",1,2,3,4,5,6);
                        }
                    }
                else
                    {
                        bootAppPrintf("open file err.\r\n",1,2,3,4,5,6);
                    }
                
            }
        else
            {
                
            }
        
        return BOOT_STATUS_COMPLETE;
        #endif
    }
LOCAL int uapprun
    (
    void* arg,
    char * cmd  /* input command string */
    )
    {
    #if 0
        rm("/tffs0/AppRun.out");
        cp("AppRun.out","/tffs0/AppRun.out");
        return BOOT_STATUS_COMPLETE;
        #else
        char* pbuf=0;
        int   len=0;
        if(userUpLoadFile("AppRun.out",&pbuf,&len)==OK)
            {
                rm("/tffs0/AppRun.out");
                int fd=open("/tffs0/AppRun.out",0x202,777);
                if(fd!=ERROR)
                    {
                       int actLen;
                       bootAppPrintf("write to flash......",1,2,3,4,5,6);
                       actLen=write(fd,pbuf,len);
                       if(actLen==len)
                        {
                            close(fd);
                            bootAppPrintf("done\r\n",1,2,3,4,5,6);
                        }
                       else
                        {
                             bootAppPrintf("write file err.\r\n",1,2,3,4,5,6);
                        }
                    }
                else
                    {
                        bootAppPrintf("open file err.\r\n",1,2,3,4,5,6);
                    }
                
            }
        else
            {
                
            }
        
        return BOOT_STATUS_COMPLETE;
        #endif

    }
#endif

LOCAL char autoboot
    (
    int timeout     /* timeout time in seconds */
    )
    {
    int     timeLeft;
    int     bytesRead = 0;
    int     i;
    FUNCPTR entry;
    char    key;
    BOOT_PARAMS oldParam;

	/*(void) ioctl (STD_IN, FIOBAUDRATE, CONSOLE_BAUD_RATE);*/
	(void) ioctl (STD_IN, FIOSETOPTIONS,
		OPT_ECHO | OPT_CRMOD | OPT_TANDEM | OPT_7_BIT);
 
    if (timeout > 0)
		{
        bootAppPrintf ("\nPress any key to stop auto-boot...\n",0,0,0,0,0,0);
	
		/* Loop looking for a char, or timeout after specified seconds */
	
        for (timeLeft = timeout; (timeLeft > 0) && (bytesRead == 0); timeLeft--)
            {
            bootAppPrintf ("%2d\r", timeLeft,0,0,0,0,0);
            for (i = 10; i > 0; i--)
                {
                (void) ioctl (STD_IN, FIONREAD, (int) &bytesRead);
                if (bytesRead != 0)
                    break;

                taskDelay(sysClkRateGet() / 10);
                }
            }
		}

    if (bytesRead == 0)    /* nothing typed so auto-boot */
		{
		/* put the console back in line mode so it echoes (so's you can bang
		 * on it to see if it's still alive) */
	
		(void) ioctl (STD_IN, FIOSETOPTIONS, OPT_TERMINAL);
	
        bootAppPrintf ("\nauto-booting...\n\n",0,0,0,0,0,0);
#if 1
 /*强制将BOOTLINE改掉，让其从TFFS加载vxworks*/
        {
            BOOT_PARAMS newparam;
            memcpy(&newparam, &sysBootParams, sizeof(BOOT_PARAMS));
            memcpy(&oldParam, &sysBootParams, sizeof(BOOT_PARAMS));
            strcpy(newparam.bootDev,"fs");
            strcpy(newparam.bootFile,"/tffs0/vxworks");
            bootStructToString (sysBootLine, &newparam);
        }
#endif    
		if (bootLoad (sysBootLine, &entry) == OK)
            {
            #if 1
                bootStructToString (sysBootLine, &oldParam);/*启动前将参数还原*/
            #endif
				printf("jump to the load address\r\n");
			    go (entry);             /* ... and never return */
            }
		else
			{
            bootAppPrintf ("Can't load boot file!!\n",0,0,0,0,0,0);
			taskDelay (sysClkRateGet ());   /* pause a second */
			reboot (BOOT_NO_AUTOBOOT);      /* something is awry */
			}
			
		/* should never get here */
		key = (char) ERROR;
		}
    else
		{
		/* read the key that stopped autoboot */
	
		read (STD_IN, &key, 1);
		key = key & 0x7f;        /* mask off parity in raw mode */
		}

	/* put it back into line mode */
    (void) ioctl (STD_IN, FIOSETOPTIONS, OPT_TERMINAL);

    return (key);
    }


static void (*orgAutoBootFunc)(int timeout); 
extern FUNCPTR bootShellAutobootFunc;

extern BOOT_PARAMS	sysBootParams;
extern int sysStartType;
LOCAL void bootAppShellAutoboot
    (
    int timeout     /* timeout time in seconds */
    )
    {
	BOOT_PARAMS params;
    timeout=3;
    char bootShellKey = 0;
 
 	/* start autoboot, unless no-autoboot specified */

    bootStringToStruct (sysBootLine, &params);
    sysFlags = params.flags;
	bootAppPrintf("cost time %d\r\n",cpu_timebase_get(), 0, 0, 0, 0, 0);
	if (!(sysStartType & BOOT_NO_AUTOBOOT) &&
 		!(sysFlags & SYSFLG_NO_AUTOBOOT))
		{
		if ((sysStartType & BOOT_QUICK_AUTOBOOT) ||
			(sysFlags & SYSFLG_QUICK_AUTOBOOT))
			{
			timeout = 1;
			}

		    bootShellKey = autoboot (timeout);   /* doesn't return if successful */
		}
		
	/* did not autoboot, return */
    bootAppPrintf ("\n",0,0,0,0,0,0);
	}


typedef struct
    {
        char* cmd;
        char* desc;
        FUNCPTR func;
        void*  arg;
    }USER_MENU;
#define USER_MENU_DEF(c,d,f,arg) {c,d,f,arg}




LOCAL void userMenuHelp(void *arg ,char* cmd);
static USER_MENU menu[]={
    USER_MENU_DEF("b","升级BOOTROM",ubootrom,0),
    USER_MENU_DEF("v","升级VXWORKS",uvxworks,0),
    USER_MENU_DEF("a","升级APPRUN",uapprun,0),
    USER_MENU_DEF("1","格式化程序FLASH",tffsFromat,0),
    USER_MENU_DEF("2","格式化数据FLASH",tffsFromat,1),
    USER_MENU_DEF("3","测试param分区",testParamGroup, 0),
    USER_MENU_DEF("4","擦除param分区",testEraseGroup, 0),
    USER_MENU_DEF("?","显示帮助",userMenuHelp,0),
    USER_MENU_DEF("q","退出",0,0), 
};
static int  memunum=sizeof(menu)/sizeof(menu[0]);

LOCAL void userMenuHelp(void *arg ,char* cmd)
{
   int i;
   for(i=0;i< memunum;i++)
    {
         USER_MENU* m=&menu[i].cmd;
         bootAppPrintf("%s -%s\r\n",m->cmd,m->desc,3,4,5,6);
    }
}


static void userMenuCall(FUNCPTR func,void* arg1,void* arg2)
{
#ifndef VX_SPE_TASK
#define VX_SPE_TASK 0
#endif
    int taskId=taskSpawn("menuFunc", 99, VX_NO_STACK_FILL|VX_FP_TASK|
                         VX_SPE_TASK, 0x50000, (FUNCPTR)func,   
                         (int)arg1, (int)arg2, 0, 0, 0, 0, 0, 0, 0, 0); 
    
    while(1)
    {
        if(ERROR==taskIdVerify(taskId))
        {
            break;  
        }
        taskDelay(sysClkRateGet());
    }
}
LOCAL int custormMenu
    (
    char * cmd  /* input command string */
    )
    {
        int i ;

        if(1)
            {
                {
                    userMenuHelp(0,0);
                }
                while(1)
               {
                    char* pLine;
                    char line[BOOT_CMD_MAX_LINE];
                    extern BOOL printBootShellPrompt;
                    if (printBootShellPrompt == TRUE)
                    {
                        bootAppPrintf ("->",0,0,0,0,0,0);
                        printBootShellPrompt = FALSE;
                    }
                   
                    extern  FUNCPTR bootAppShellFunc;
                    pLine=bootAppShellFunc(line,BOOT_CMD_MAX_LINE);
                     if (strlen(pLine) > 0)
                        {

                                for(i=0;i<memunum;i++)
                                {
                                     USER_MENU* m=&menu[i].cmd;
                                     if(strcmp(m->cmd,pLine)==0)
                                        {
                                            if(m->func)
                                                {
                                                    bootAppPrintf("->%s.....\r\n",m->desc,2,3,4,5,6);
                                                    userMenuCall(m->func,m->arg,pLine);
                                                    /*m->func(m->arg,pLine);*/
                                                    break;
                                                }
                                            else
                                                {
                                                    goto done;
                                                }
                                            
                                        }
                                }               
                        }
                     else
                        {
                           
                        }
                   
               }
                
            }

        done:
        return BOOT_STATUS_COMPLETE;
    }




void bootXSysCmdInit(void)
    {

    /* Add commands to boot shell */
    bootCommandHandlerAdd("o", custormMenu, BOOT_CMD_MATCH_STRICT,
 		"o", "- 用户菜单");
    

    bootCommandHandlerAdd("md", mtdShowAll, BOOT_CMD_MATCH_STRICT,
 		"md", "- show  the mtd");
    
     bootCommandHandlerAdd("ubootrom", ubootrom, BOOT_CMD_MATCH_STRICT,
    		"ubootrom", "- update bootloader");   

     orgAutoBootFunc=bootShellAutobootFunc;
     bootShellAutobootFunc=bootAppShellAutoboot;
}


