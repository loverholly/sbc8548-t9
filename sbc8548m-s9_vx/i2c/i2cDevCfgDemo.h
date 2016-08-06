
#ifdef INCLUDE_ZMD_I2C  /*for 82xx*/
	struct hcfResource i2cx0Resources[] =
	{
	    { "irq",           	HCF_RES_INT,   	{(void *)INUM_I2C}},
		{ "predivider",    	HCF_RES_INT,   	{(void *)((UINT32)16)}},/*I2C ��������Ԥ��Ƶϵ��*/
		{ "workMode",    	HCF_RES_INT,   	{(void *)((UINT32)1)}},/*1Ϊmasterģʽ 0 Ϊslaveģʽ*/
#if 0   /*��ѡ�� ������ʹ�����²�����ΪĬ������*/
		{ "i2cSpeed",    	HCF_RES_INT,   	{(void *)((UINT32)100000)}},
		{ "maxBufLen",    	HCF_RES_INT,   	{(void *)((UINT32)16*1)}},/*ȡ��BDxSpace��ͬ��С����*/
		{ "BDxSpace",    	HCF_RES_INT,   	{(void *)((UINT32)16*1)}},/*16 * n*/
		{ "BDxTxNum",    	HCF_RES_INT,   	{(void *)((UINT32)2*1)}},/*1 ��������2TXBD  ����������Ҫ����txBD����� ����ʵ����ֻ��ʹ��һ��BD*/
		{ "BDxRxNum",    	HCF_RES_INT,   	{(void *)((UINT32)1*1)}},/*2 1Rx*/
		{ "BDxbufWidth",    HCF_RES_INT,   	{(void *)((UINT32)64*4)}},/*64 * n*/
#endif
	};
	#define i2cx0Num NELEMENTS(i2cx0Resources)
#endif

#ifdef INCLUDE_ZMD_I2C/*for 83xx 85xx 86xx*/
	struct hcfResource i2cx0Resources[] =
	{
	    { "cs",             HCF_RES_INT,  {(void *)0} },/*i2c������map*/
	    { "irq",            HCF_RES_INT,   {(void *)INUM_I2C1} },/*�жϺ�*/
	};
	#define i2cx0Num NELEMENTS(i2cx0Resources)

	struct hcfResource i2cx1Resources[] =
	{
	    { "cs",     HCF_RES_INT,   {(void *)1} },/*i2c������map 0  83xXֻ��һ��*/
	    { "irq",    HCF_RES_INT,   {(void *)INUM_I2C2} },/*�жϺ�*/
	};
	#define i2cx1Num NELEMENTS(i2cx1Resources)
#endif


/* �豸�����б� */
struct hcfDevice hcfDeviceList[] = 
{

#ifdef INCLUDE_ZMD_I2C
	{  "i2c",			0,	VXB_DEVID_I2C,	0,	i2cx0Num,	i2cx0Resources},
#endif

};

int hcfDeviceNum = NELEMENTS(hcfDeviceList);

