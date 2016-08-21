#include <stdio.h>
#include <vxworks.h>

unsigned char usrExtDioStatusGet(unsigned char port)
{
	return ((*(volatile unsigned char *)(0xf0100009))&(1<<((port&0x3)+5))) != 0;
}


void usrExtLedCtl(unsigned char port, unsigned int off)
{
	volatile unsigned char status;
	
	status = *(volatile unsigned char *)(0xf0100009);
	
	if (off)
		*(volatile unsigned char *)(0xf0100009) = status|(1 << (port&0x1));
	else
		*(volatile unsigned char *)(0xf0100009) = status&(~(1 << (port&0x1)));
}
