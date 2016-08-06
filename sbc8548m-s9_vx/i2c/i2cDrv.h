/**
 * @file   i2cDrv.h
 * @author  liujie@LIUJIE
 * @date   Tue Jul 28 14:13:02 2013
 * 
 * @brief  I2C驱动接口定义
 * 
 * 
 */

#ifndef __ZMD_I2C_DRV_H__
#define __ZMD_I2C_DRV_H__
#include <vxWorks.h>

typedef enum{
    I2C_CMD_BAUD_SET = 1,       /**< 设置波特率 */
    I2C_CMD_BAUD_GET            /**< 获取波特率 */
}I2C_CMD_T;

/** 
 * 读取i2c数据
 * @param busIndex 总线号
 * @param devAddr 设备地址
 * @param regAddrArray 读取过程是先写出寄存器地址，寄存器地址位数不定，因此传入的是一个字节流
 * @param regAddrLen 寄存器地址的长度，也就是写入的regAddrArray的长度，单位:byte
 * @param pData 读取数据buffer的指针
 * @param len 读取的长度，单位Byte
 * @param waitRtn 某些i2c设备在写完寄存器地址后需要等待固定的时间或是查询某个状态，waitRtn可以实现这种操作，如果不需要该函数，传入NULL
 * 
 * @return OK(0):成功  <0:失败
 */
int i2cRead(int busIndex, int devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn);

/** 
 * 写入i2c数据
 * 
 * @param busIndex 控制器index
 * @param devAddr 设备地址
 * @param regAddrArray 读取过程是先写出寄存器地址，寄存器地址位数不定，因此传入的是一个字节流
 * @param regAddrLen 寄存器地址的长度，也就是写入的regAddrArray的长度，单位:byte
 * @param pData 写入数据的指针
 * @param len 写入的数据长度，单位Byte
 * @param waitRtn 某些i2c设备在写完寄存器地址后需要等待固定的时间或是查询某个状态，waitRtn可以实现这种操作，如果不需要该函数，传入NULL
 * 
 * @return OK(0):成功  <0:失败
 */
int i2cWrite(int busIndex, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn);


/** 
 * 配置i2c参数
 * @param busIndex 控制器index
 * @param cmd 命令类型，详见@ref I2C_CMD_T
 * @param arg 命令参数
 * 
 * @return OK(0):成功  <0:失败
 */

int i2cCtrl(int busIndex, I2C_CMD_T cmd, unsigned int arg);


#endif
