/**
 * @file   i2cDrv.h
 * @author  liujie@LIUJIE
 * @date   Tue Jul 28 14:13:02 2013
 * 
 * @brief  I2C�����ӿڶ���
 * 
 * 
 */

#ifndef __ZMD_I2C_DRV_H__
#define __ZMD_I2C_DRV_H__
#include <vxWorks.h>

typedef enum{
    I2C_CMD_BAUD_SET = 1,       /**< ���ò����� */
    I2C_CMD_BAUD_GET            /**< ��ȡ������ */
}I2C_CMD_T;

/** 
 * ��ȡi2c����
 * @param busIndex ���ߺ�
 * @param devAddr �豸��ַ
 * @param regAddrArray ��ȡ��������д���Ĵ�����ַ���Ĵ�����ַλ����������˴������һ���ֽ���
 * @param regAddrLen �Ĵ�����ַ�ĳ��ȣ�Ҳ����д���regAddrArray�ĳ��ȣ���λ:byte
 * @param pData ��ȡ����buffer��ָ��
 * @param len ��ȡ�ĳ��ȣ���λByte
 * @param waitRtn ĳЩi2c�豸��д��Ĵ�����ַ����Ҫ�ȴ��̶���ʱ����ǲ�ѯĳ��״̬��waitRtn����ʵ�����ֲ������������Ҫ�ú���������NULL
 * 
 * @return OK(0):�ɹ�  <0:ʧ��
 */
int i2cRead(int busIndex, int devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn);

/** 
 * д��i2c����
 * 
 * @param busIndex ������index
 * @param devAddr �豸��ַ
 * @param regAddrArray ��ȡ��������д���Ĵ�����ַ���Ĵ�����ַλ����������˴������һ���ֽ���
 * @param regAddrLen �Ĵ�����ַ�ĳ��ȣ�Ҳ����д���regAddrArray�ĳ��ȣ���λ:byte
 * @param pData д�����ݵ�ָ��
 * @param len д������ݳ��ȣ���λByte
 * @param waitRtn ĳЩi2c�豸��д��Ĵ�����ַ����Ҫ�ȴ��̶���ʱ����ǲ�ѯĳ��״̬��waitRtn����ʵ�����ֲ������������Ҫ�ú���������NULL
 * 
 * @return OK(0):�ɹ�  <0:ʧ��
 */
int i2cWrite(int busIndex, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, char * pData, int len, FUNCPTR waitRtn);


/** 
 * ����i2c����
 * @param busIndex ������index
 * @param cmd �������ͣ����@ref I2C_CMD_T
 * @param arg �������
 * 
 * @return OK(0):�ɹ�  <0:ʧ��
 */

int i2cCtrl(int busIndex, I2C_CMD_T cmd, unsigned int arg);


#endif
