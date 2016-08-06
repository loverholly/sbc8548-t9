/**
 * @file   i2cDrv.c
 * @author  liujie@LIUJIE
 * @date   Thu Jan 23 13:52:57 2014
 * 
 * @brief  平台化的i2c设备的驱动接口实现文件
 * @todo 82xx平台上面的i2c驱动，各平台上面的调试功能
 * @detail I2C驱动通用接口实现
 * - 驱动特性
 *  - 仅只持i2c主模式，每一个i2c控制器对应一个控制器ID(busIndex), 通过控制器对I2C设备进行读写都基于busIndex, 从0开始编号。
 *  - 设备默认波特率为400Kbps
 * - 驱动能力
 *  - 读写I2C设备
 *  - 设置/获取I2C控制器波特率
 * - 读写方式
 *  - i2c从设备通过设备地址进行区分，地址范围为0-127
 *  - i2c设备的
 *  - 因为不同的设备的寄存器地址长度有可能不同，因此读写时寄存器地址通过一个数组传递给底层，同时制定地址的长度,1表示8bit寄存器地址,2表示16bit.
 *  - 读写的数据也是通过指定长度数组的数组进行定义。
 *  - 读写有两种方式
 *   - 阻塞式:数据IO结束后读写函数才返回
 */

/**
 * @page page_i2c_manual I2C
 * - 能力:支持I2C主模式，可以对外部连接的设备进行读写操作。
 * - I2C主设备接口详见@ref i2cDrv.h
 * - I2C控制器类型
 *  - @subpage page_i2c_fsl_cfg_demo
 */

#include "i2cDrv.h"
#include "i2cP.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>

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



#define MAX_I2C_CONTROLLER_NUM 4

struct i2c_dev{
    UINT32 hwHandle;
    struct i2c_routine routines;
    SEM_ID sem;
};


static struct i2c_dev * __i2c_devs[MAX_I2C_CONTROLLER_NUM]= {NULL, NULL, NULL, NULL};



int i2cRegister(int index, unsigned int ctrlHandle, struct i2c_routine * routines)
{
    if(index <0 || index > MAX_I2C_CONTROLLER_NUM){
        PERROR("index %d illegal", index);
        return ERROR;
    }
    if((__i2c_devs[index])){
        PERROR("控制器%d已经存在", index);
        return ERROR;
    }
    __i2c_devs[index] = (struct i2c_dev *)malloc(sizeof(*__i2c_devs[index]));
    if(!__i2c_devs[index])
        return -ENOMEM;
    memset(__i2c_devs[index], 0, sizeof(*__i2c_devs[index]));
    memcpy((char*)&__i2c_devs[index]->routines, (char *)routines, sizeof(*routines));
    __i2c_devs[index]->hwHandle = ctrlHandle;
    return OK;
}


int i2cRead(int busIndex, int devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn)
{
    int ret;
    if(busIndex >= MAX_I2C_CONTROLLER_NUM || !__i2c_devs[busIndex]){
        PERROR("i2c控制器%d不存在", busIndex);
        return -EINVAL;
    }
    semTake(__i2c_devs[busIndex]->sem, WAIT_FOREVER);
    ret = __i2c_devs[busIndex]->routines.read(busIndex, devAddr, regAddrArray, regAddrLen, pData, len, waitRtn);
    semGive(__i2c_devs[busIndex]->sem);
    return ret;
}

int i2cWrite(int busIndex, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn)
{
    int ret;
    if(busIndex >= MAX_I2C_CONTROLLER_NUM || !__i2c_devs[busIndex]){
        PERROR("i2c控制器%d不存在", busIndex);
        return -EINVAL;
    }
    semTake(__i2c_devs[busIndex]->sem, WAIT_FOREVER);
    ret = __i2c_devs[busIndex]->routines.write(busIndex, devAddr, regAddrArray, regAddrLen, pData, len, waitRtn);
    semGive(__i2c_devs[busIndex]->sem);
    return ret;
}

int i2cCtrl(int busIndex, I2C_CMD_T cmd, unsigned int arg)
{
    int ret;
    if(busIndex >= MAX_I2C_CONTROLLER_NUM || !__i2c_devs[busIndex]){
        PERROR("i2c控制器%d不存在", busIndex);
        return -EINVAL;
    }
    semTake(__i2c_devs[busIndex]->sem, WAIT_FOREVER);
    ret = __i2c_devs[busIndex]->routines.ctrl(busIndex, cmd, arg);
    semGive(__i2c_devs[busIndex]->sem);
    return ret;
    
}
