
#ifdef INCLUDE_ZMD_I2C  /*for 82xx*/
	struct hcfResource i2cx0Resources[] =
	{
	    { "irq",           	HCF_RES_INT,   	{(void *)INUM_I2C}},
		{ "predivider",    	HCF_RES_INT,   	{(void *)((UINT32)16)}},/*I2C 控制器的预分频系数*/
		{ "workMode",    	HCF_RES_INT,   	{(void *)((UINT32)1)}},/*1为master模式 0 为slave模式*/
#if 0   /*可选项 不配置使用以下参数作为默认配置*/
		{ "i2cSpeed",    	HCF_RES_INT,   	{(void *)((UINT32)100000)}},
		{ "maxBufLen",    	HCF_RES_INT,   	{(void *)((UINT32)16*1)}},/*取与BDxSpace相同大小即可*/
		{ "BDxSpace",    	HCF_RES_INT,   	{(void *)((UINT32)16*1)}},/*16 * n*/
		{ "BDxTxNum",    	HCF_RES_INT,   	{(void *)((UINT32)2*1)}},/*1 至少两个2TXBD  接收数据需要两个txBD来完成 发送实际上只能使用一个BD*/
		{ "BDxRxNum",    	HCF_RES_INT,   	{(void *)((UINT32)1*1)}},/*2 1Rx*/
		{ "BDxbufWidth",    HCF_RES_INT,   	{(void *)((UINT32)64*4)}},/*64 * n*/
#endif
	};
	#define i2cx0Num NELEMENTS(i2cx0Resources)
#endif

#ifdef INCLUDE_ZMD_I2C/*for 83xx 85xx 86xx*/
	struct hcfResource i2cx0Resources[] =
	{
	    { "cs",             HCF_RES_INT,  {(void *)0} },/*i2c控制器map*/
	    { "irq",            HCF_RES_INT,   {(void *)INUM_I2C1} },/*中断号*/
	};
	#define i2cx0Num NELEMENTS(i2cx0Resources)

	struct hcfResource i2cx1Resources[] =
	{
	    { "cs",     HCF_RES_INT,   {(void *)1} },/*i2c控制器map 0  83xX只有一个*/
	    { "irq",    HCF_RES_INT,   {(void *)INUM_I2C2} },/*中断号*/
	};
	#define i2cx1Num NELEMENTS(i2cx1Resources)
#endif


/* 设备定义列表 */
struct hcfDevice hcfDeviceList[] = 
{

#ifdef INCLUDE_ZMD_I2C
	{  "i2c",			0,	VXB_DEVID_I2C,	0,	i2cx0Num,	i2cx0Resources},
#endif

};

int hcfDeviceNum = NELEMENTS(hcfDeviceList);

