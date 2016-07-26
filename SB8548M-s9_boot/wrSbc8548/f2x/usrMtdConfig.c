#include "vxworks.h"
#include "config.h"

/* global defintion */
extern STATUS sysTffsFormat (int tffsNo);
#define F2X_MTD_MAX_NUMBER 4
#define MAX_NORFLASH_TFFS_PARTITION 4

F2X_GID f2xMtd[F2X_MTD_MAX_NUMBER] = {NULL, NULL, NULL, NULL};

F2X_GID f2xTffsGid[MAX_NORFLASH_TFFS_PARTITION] = {(F2X_GID)NULL, (F2X_GID)NULL, (F2X_GID)NULL};

unsigned int f2xFlashBase()
{
	return FLASH1_BASE_ADRS;
}

unsigned int f2xFlashSize()
{
	return 0x4000000;	/* tmd,K9上面只用了32M不是64M,shit */
}

STATUS  f2xMtdRegister()
    {
        static int i=0;
        int  wbCapbility=0;
        unsigned int adrs_part[4]={0};
		#define BOOTROM_SPACE_SIZE 0x100000
		#define PARAM_SPACE_SIZE   0x100000
		#define VXWORKS_SPACE_SIZE 0x800000
        unsigned int adrs_size[4]={BOOTROM_SPACE_SIZE,PARAM_SPACE_SIZE,VXWORKS_SPACE_SIZE,0};

        
        adrs_part[0]=f2xFlashBase()-adrs_size[0]+f2xFlashSize();/*bootrom*/
        adrs_part[1]=adrs_part[0]-adrs_size[1];/*NVRAM*/
        adrs_part[2]=adrs_part[1]-adrs_size[2];/*VXWORKS & APP*/
        adrs_part[3]=f2xFlashBase();
        adrs_size[3]=adrs_part[2]-adrs_part[3];
        /*adrs_size[3]=0x400000;*/
        for(i=0;i<4;i++)
            {
              F2X_GID gid;
			  F2X_RID rid;
			  if (i == 0)
			  {
				  rid = f2xRegionCreate((char*)0xfff00000,
										2,
										0x100000, /* bootrom空间为1Mbyte */
										2,
										0x4000, /* K9上面的flash大小为1Mbyte和64Mbyte结构 */
				                        2, F29_16BIT_TYPE | wbCapbility,
										NULL, (VOIDFUNCPTR) NULL, NONE);    
			  }
			  else
			  {
				  rid = f2xRegionCreate(
                        (char *)adrs_part[i],/* 起始地址 */
                        2,
                        adrs_size[i], /* region 大小 */
                        2,
                        0x20000,
                        2, F29_16BIT_TYPE | wbCapbility,
                        NULL, (VOIDFUNCPTR) NULL, NONE);
			  }
               if (!rid)
                {
                    printf("create region 0x%x size 0x%x failed", adrs_part[i],adrs_size[i]);
                    return ERROR;
                }
               gid=f2xGroupCreate(rid);
               if (!gid)
                {
            	   printf("create group 0x%x size 0x%x failed", adrs_part[i],adrs_size[i]);
                    return ERROR;
                } 
               f2xMtd[i]=gid;

               if(i==1) /*NVRAM*/
                {
                    sysNvRamAdd(rid);
                }
              
            }

         f2xRegister(f2xMtd[2]);/*register vxworks ,app as tffs*/
         f2xRegister(f2xMtd[3]);/*register tffs*/
        
        
    }

STATUS f2xSet
(
    int   i,        /* group ID */
    char *  pSrc,      /* source buffer */
    int     nbytes,    /* number of bytes to copy */
    int     offset     /* byte offset into group */
 )
{
	int ret=ERROR;
	if(f2xMtd[i]!=0)
	{
		ret= f2xGroupSet(f2xMtd[i], pSrc, nbytes, offset);
	}
	return ret;
}
STATUS f2xGet
(
    int   i,        /* group ID */
    char *  pDst,      /* source buffer */
    int     nbytes,    /* number of bytes to copy */
    int     offset     /* byte offset into group */
 )
{
	int ret=ERROR;
	if(f2xMtd[i]!=0)
	{
		ret= f2xGroupGet(f2xMtd[i], pDst, nbytes, offset);
	}
	return ret;        
}



void mtdShow()
{
    int i;
    for(i=0; i<F2X_MTD_MAX_NUMBER; i++){
        if ((f2xMtd[i])) f2xGroupShow(f2xMtd[i]);
    }
}

/*******************************************************************************
 *
 * sysTffsInit - board level initialization for TFFS
 *
 * This routine calls the socket registration routines for the socket component
 * drivers that will be used with this BSP. The order of registration signifies
 * the logical drive number assigned to the drive associated with the socket.
 *
 * RETURNS: N/A
 */

LOCAL void sysTffsInit (void)
{

    UINT32 ix = 0;
    UINT32 iy = 1;
    UINT32 iz = 2;
    int oldTick;

		/*
		 * we assume followings:
		 *   - no interrupts except timer is happening.
		 *   - the loop count that consumes 1 msec is in 32 bit.
		 * it is done in the early stage of usrRoot() in tffsDrv().
		 */

    oldTick = tickGet();
    while (oldTick == tickGet())    /* wait for next clock interrupt */
        ;

    oldTick = tickGet();
    while (oldTick == tickGet())    /* loop one clock tick */
	{
        iy = KILL_TIME_FUNC;        /* consume time */
        ix++;                       /* increment the counter */
	}

    sysTffsMsecLoopCount = ix * sysClkRateGet() / 1000;

		/* registers the MTD drivers according to it's Hardware Geometry */
	f2xMtdRegister();
 
	/*sysTffsFormat(0);*/
}



#include "f2xFlashMem.h"
 
int sysTffsProgressCb (int totalUnitsToFormat, int totalUnitsFormattedSoFar)
{
    printf("Formatted %d of %d units = %d.%01d %%\r",
           totalUnitsFormattedSoFar, totalUnitsToFormat,
           100 * totalUnitsFormattedSoFar / totalUnitsToFormat,
           1000 * totalUnitsFormattedSoFar / totalUnitsToFormat % 10
           );
    if (totalUnitsFormattedSoFar == totalUnitsToFormat)
        printf ("\n");
    return flOK;
}

STATUS sysTffsFormat (int tffsNo)
{
    STATUS status;
        /* No fallow area */
    tffsDevFormatParams params = 
        {
            {0x000000l, 99, 1,  0x10000l, sysTffsProgressCb, {0,0,0,0}, NULL, 2, 0, NULL}, 
            FTL_FORMAT
        } ;

    if(tffsNo > noOfDrives)
        return ERROR;
	
        /* we assume that the drive number 0 is RFA */

    printf("Formatting volume %d...",tffsNo);
	
    status = tffsDevFormat (tffsNo, (int)&params);
    if ( status != OK )
    {
        printf("ERROR.\n");
        return ERROR ;
    }
    printf("OK.\n");
	
    return(OK);
}

STATUS sysTffsDosFormat(int i)
{
        
	char path[32];
	sprintf(path,"/tffs%d",i);
	if(OK!=sysTffsFormat(i))
	{
		printf("格式化%s失败\n", path);
		return ERROR;              
	}
	if (OK != usrTffsConfig(i, 0, path)){
		printf("挂载%s失败\n", path);
		return ERROR;
	}
	if (OK != dosFsVolFormat(path, 0, NULL)){
		printf("dos格式化%s失败\n", path);
		return ERROR;
	}
	return OK;
}


