#ifndef __I2C_DRV_P_H__
#define __I2C_DRV_P_H__
#include <vxWorks.h>
struct i2c_routine{
    int (*read)(int ctrlHandle, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, 
                      char * pData, int len, FUNCPTR waitRtn);
    int (*write)(int ctrlHandle, UINT8 devAddr, UINT8 * regAddrArray, int regAddrLen, 
                      char * pData, int len, FUNCPTR waitRtn);
    int (*ctrl)(int ctrlHandle, int cmd, unsigned int arg);
};

int i2cRegister(int index, unsigned int ctrlHandle, struct i2c_routine * routines);


#endif
