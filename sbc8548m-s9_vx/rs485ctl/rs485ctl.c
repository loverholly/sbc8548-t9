#include <vxworks.h>
#include <stdio.h>
#include <stdlib.h>
#include <ioLib.h>

#define CPLD_RS485_CTRL_REG (0xf0100008)
/**
 *  @brief  usrRs485DirectionSet
 *  @param  port-0~3;direction--0��ʾ����,1��ʾ����
 *  @return void
 */
void usrRs485DirectionSet(unsigned char port, unsigned char direction)
{
	unsigned char readVal = 0;
	readVal = *(volatile unsigned char *)(CPLD_RS485_CTRL_REG);
	port = port&0x3;
	if (direction) {
		*(volatile unsigned char *)(CPLD_RS485_CTRL_REG) =
			readVal|(1 << port);
	} else {
		*(volatile unsigned char *)(CPLD_RS485_CTRL_REG) =
			readVal&(~(1<<port));
	}
	
	return;
}
