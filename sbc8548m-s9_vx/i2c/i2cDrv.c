/**
 * @file   i2cDrv.c
 * @author  liujie@LIUJIE
 * @date   Thu Jan 23 13:52:57 2014
 * 
 * @brief  ƽ̨����i2c�豸�������ӿ�ʵ���ļ�
 * @todo 82xxƽ̨�����i2c��������ƽ̨����ĵ��Թ���
 * @detail I2C����ͨ�ýӿ�ʵ��
 * - ��������
 *  - ��ֻ��i2c��ģʽ��ÿһ��i2c��������Ӧһ��������ID(busIndex), ͨ����������I2C�豸���ж�д������busIndex, ��0��ʼ��š�
 *  - �豸Ĭ�ϲ�����Ϊ400Kbps
 * - ��������
 *  - ��дI2C�豸
 *  - ����/��ȡI2C������������
 * - ��д��ʽ
 *  - i2c���豸ͨ���豸��ַ�������֣���ַ��ΧΪ0-127
 *  - i2c�豸��
 *  - ��Ϊ��ͬ���豸�ļĴ�����ַ�����п��ܲ�ͬ����˶�дʱ�Ĵ�����ַͨ��һ�����鴫�ݸ��ײ㣬ͬʱ�ƶ���ַ�ĳ���,1��ʾ8bit�Ĵ�����ַ,2��ʾ16bit.
 *  - ��д������Ҳ��ͨ��ָ�����������������ж��塣
 *  - ��д�����ַ�ʽ
 *   - ����ʽ:����IO�������д�����ŷ���
 */

/**
 * @page page_i2c_manual I2C
 * - ����:֧��I2C��ģʽ�����Զ��ⲿ���ӵ��豸���ж�д������
 * - I2C���豸�ӿ����@ref i2cDrv.h
 * - I2C����������
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
        PERROR("������%d�Ѿ�����", index);
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
        PERROR("i2c������%d������", busIndex);
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
        PERROR("i2c������%d������", busIndex);
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
        PERROR("i2c������%d������", busIndex);
        return -EINVAL;
    }
    semTake(__i2c_devs[busIndex]->sem, WAIT_FOREVER);
    ret = __i2c_devs[busIndex]->routines.ctrl(busIndex, cmd, arg);
    semGive(__i2c_devs[busIndex]->sem);
    return ret;
    
}
