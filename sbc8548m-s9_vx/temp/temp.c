#include <vxworks.h>

int t75(int deviceaddr)
{
    unsigned char buf[2] = {0, 0};
    float temperature = 0.0;
    UINT16 temp = 0;

    fslI2cRead(0, deviceaddr, 0, (char *)buf, 2);
    printf("TEMPERATURE REGISTER D15-D8 = 0x%x\n", buf[0]);
    printf("TEMPERATURE REGISTER D7-D0 = 0x%x\n", buf[1]);

    temp = ((((UINT16)buf[0] << 8) | buf[1]) >> 7) & 0x1ff;
    temperature = (float)temp * 0.5;
    printf("Temperature = %f\n", temperature);

    fslI2cRead(0, deviceaddr, 1, (char *)buf, 1);
    printf("CONFIGURATION REGISTER = 0x%x\n", buf[0]);

    fslI2cRead(0, deviceaddr,  2, (char *)buf, 2);
    printf("THYST D15-D8 = 0x%x\n", buf[0]);
    printf("THYST D7-D0 = 0x%x\n", buf[1]);

    fslI2cRead(0, deviceaddr, 3, (char *)buf, 2);
    printf("TOS D15-D8 = 0x%x\n", buf[0]);
    printf("TOS D7-D0 = 0x%x\n", buf[1]);
    
    return 0;
}


float usrGetTemp(void)
{
    unsigned char buf[2] = {0, 0};
    float temperature = 0.0;
    UINT16 temp = 0;

    fslI2cRead(0, 0x48, 0, (char *)buf, 2);

    temp = ((((UINT16)buf[0] << 8) | buf[1]) >> 7) & 0x1ff;
    if (temp & 0x100) {
        temperature = (float)(((~temp) + 1) & 0x1ff) * (-0.5);
    }
    else {
        temperature = (float)temp * 0.5;
    }

    return temperature;
}


void testTemp(void)
{
	printf("temp %f\r\n", usrGetTemp());
}
