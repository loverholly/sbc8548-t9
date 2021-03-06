/* hwconf.c - Hardware configuration support module */

/*
 * Copyright (c) 2006-2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01k,24oct08,y_w  Fixed the build error when INCLUDE_RAPIDIO_BUS is defined.
                 (WIND00140932)
01j,20jun08,wap  Increase maximum bus count from 3 to 16
01h,06jun08,dlk  Make entries for e/tsec devices unconditional.
                 Support END2 versions of the drivers also.
01i,20sep07,b_m  add ns16550, ppcIntCtlr, epic, m85xxTimer vxbus drivers.
01h,10sep07,wap  Switch to VxBus PCI driver (WIND00104076)
01g,07aug07,dtr  Update for latest RIO driver.
01f,02aug07,h_k  added LAWAR_ENABLE and LAWBAR_ADRS_SHIFT.
01e,31jul07,agf  change tsec hEND to vxBus END
01d,09apr07,b_m  modify to use m85xxCCSR driver.
01c,09mar06,wap  Allow either mottsec or motetsec driver to be used
                 (SPR #118829)
01b,07feb06,wap  Add VxBus parameter table
01a,16jan06,dtr  written.
*/

#include <vxWorks.h>
#include <vxBusLib.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPlbLib.h>
#include <hwif/vxbus/hwConf.h>
#include <../src/hwif/h/vxbus/vxbRapidIO.h>
#include <hwif/util/vxbParamSys.h>
#include <hwif/vxbus/vxbIntrCtlr.h>
#include <hwif/intCtlr/vxbEpicIntCtlr.h>
#include "config.h"
#include "mot85xxPci.h"

#define XTSEC_NAME "mottsec"
#if defined (INCLUDE_ETSEC_VXB_END) || defined (INCLUDE_ETSEC_VXB_END2)
#  undef  XTSEC_NAME
#  define XTSEC_NAME "motetsec"
#endif /* defined(INCLUDE_ETSEC_VXB_END) || defined(INCLUDE_ETSEC_VXB_END2) */
/* #define INCLUDE_OPTIONAL_TSECS */
/*
 * On the SBC8548 board, PHYs are physically wired according
 * to the following table:
 *
 * 		Data port pins	Management port pins	MII address
 *		--------------	--------------------	-----------
 * PHY0:	TSEC0		TSEC0			25
 * PHY1:	TSEC1		TSEC0			26
 * PHY2: (note) TSEC2           TSEC0                   27 (note)
 * PHY3: (note) TSEC3           TSEC0                   28 (note)
 *
 * The tricky part is that both PHYs have their management pins
 * connected to TSEC0. The VxBus TSEC driver allows us to specify
 * the name of the device that provides PHY management support.
 * This device is "mottsec0" in both cases. We also specify the
 * "phyAddr" resource for each device, which is 25 for TSEC0
 * and 26 for TSEC1.
 *
 * Note: PHY2 and PHY3 do not exist on the SBC8548, but the corresponding
 * TSEC signals are routed to a high-speed expansion connector. So, as a
 * placeholder for the following structures, addresses 27 and 28 were
 * entered for the phyAddr fields.
 *
 */

const struct hcfResource tsecVxbEnd0Resources[] = {
    { "regBase",        HCF_RES_INT,    { (void *)(CCSBAR + 0x24000) } },
    { "phyAddr",	    HCF_RES_INT,	{ (void *)1 } },
    { "miiIfName",          HCF_RES_STRING, { (void *)"motetsec" } },
    { "miiIfUnit",      HCF_RES_INT,    { (void *)0 } }
};
#define tsecVxbEnd0Num NELEMENTS(tsecVxbEnd0Resources)

const struct hcfResource tsecVxbEnd1Resources[] = {
    { "regBase",        HCF_RES_INT,    { (void *)(CCSBAR + 0x25000) } },
    { "phyAddr",	    HCF_RES_INT,	{ (void *)2} },
    { "miiIfName",          HCF_RES_STRING, { (void *)"motetsec" } },
    { "miiIfUnit",      HCF_RES_INT,    { (void *)0 } }
};
#define tsecVxbEnd1Num NELEMENTS(tsecVxbEnd1Resources)

const struct hcfResource tsecVxbEnd2Resources[] = {
    { "regBase",        HCF_RES_INT,    { (void *)(CCSBAR + 0x26000) } },
    { "phyAddr",	    HCF_RES_INT,	{ (void *)3 } },
    { "miiIfName",          HCF_RES_STRING, { (void *)"motetsec" } },
    { "miiIfUnit",      HCF_RES_INT,    { (void *)0 } },
};
#define tsecVxbEnd2Num NELEMENTS(tsecVxbEnd2Resources)

const struct hcfResource tsecVxbEnd3Resources[] = {
    { "regBase",        HCF_RES_INT,    { (void *)(CCSBAR + 0x27000) } },
    { "phyAddr",	    HCF_RES_INT,	{ (void *)4 } },
    { "miiIfName",          HCF_RES_STRING, { (void *)"motetsec" } },
    { "miiIfUnit",      HCF_RES_INT,    { (void *)0 } },
};
#define tsecVxbEnd3Num NELEMENTS(tsecVxbEnd3Resources)

const struct hcfResource mv88e1x11fixup0[] = {
	{ "phyAddr",            HCF_RES_INT, { (void *)1 } },
	{ "nonGenericBspMod",   HCF_RES_INT, { (void *)1 } },
};

const struct hcfResource mv88e1x11fixup1[] = {
	{ "phyAddr",            HCF_RES_INT, { (void *)2 } },
	{ "nonGenericBspMod",   HCF_RES_INT, { (void *)1 } },
};

const struct hcfResource mv88e1x11fixup2[] = {
	{ "phyAddr",            HCF_RES_INT, { (void *)3 } },
	{ "nonGenericBspMod",   HCF_RES_INT, { (void *)1 } },
};

const struct hcfResource mv88e1x11fixup3[] = {
	{ "phyAddr",            HCF_RES_INT, { (void *)4 } },
	{ "nonGenericBspMod",   HCF_RES_INT, { (void *)1 } },
};

#ifdef INCLUDE_RAPIDIO_BUS
const struct hcfResource m85xxRio0Resources[] = {
    { "regBase",	HCF_RES_INT, {(void *)RAPIDIO_BASE} },
    { "deviceBase", HCF_RES_INT, {(void *)(RAPIDIO_ADRS + 0x0000000)}},
    { "deviceSize", HCF_RES_INT, {(void *)(RAPIDIO_SIZE - 0x0000000)}},
    { "rioBusAdrs", HCF_RES_INT, {(void *)RAPIDIO_BUS_ADRS }},
    { "rioBusSize", HCF_RES_INT, {(void *)RAPIDIO_BUS_SIZE }},
    { "tgtIf",      HCF_RES_INT, {(void *)LAWAR_TGTIF_RAPIDIO }},
    { "localMemAdrs",HCF_RES_INT, {(void *)LOCAL_MEM_LOCAL_ADRS}}
};
#define m85xxRio0Num    NELEMENTS(m85xxRio0Resources)

const struct hcfResource m85xxCPU0Resources[] = {
	{ "regBase",	HCF_RES_INT, {(void *)RAPIDIO_BASE }},
	{ "targetID",	HCF_RES_INT, {(void *)0 }},
	{ "outboundWindow0",     HCF_RES_INT, {(void *)RIO_CHANNEL_RESERVED }},
	{ "outboundWindow1",     HCF_RES_INT, {(void *)RIO_CHANNEL_MAINT }},
	{ "outboundWindow2",     HCF_RES_INT, {(void *)RIO_CHANNEL_CFG }},
	{ "outboundWindow3",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow4",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow5",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow6",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow7",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow8",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow0",     HCF_RES_INT, {(void *)RIO_CHANNEL_RESERVED }},
	{ "inboundWindow1",     HCF_RES_INT, {(void *)RIO_CHANNEL_SM }},
	{ "inboundWindow2",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow3",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow4",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }}

};
#define m85xxCPU0Num    NELEMENTS(m85xxCPU0Resources)

const struct hcfResource m85xxCPU1Resources[] = {
	{ "regBase",	HCF_RES_INT, {(void *)RAPIDIO_BASE }},
	{ "targetID",	HCF_RES_INT, {(void *) 0x9 }},
	{ "hopCount",	HCF_RES_INT, {(void *) 0x0 }},
	{ "outboundWindow0",     HCF_RES_INT, {(void *)RIO_CHANNEL_RESERVED }},
	{ "outboundWindow1",     HCF_RES_INT, {(void *)RIO_CHANNEL_SM }},
	{ "outboundWindow2",     HCF_RES_INT, {(void *)RIO_CHANNEL_TAS_SET }},
	{ "outboundWindow3",     HCF_RES_INT, {(void *)RIO_CHANNEL_TAS_CLEAR }},
	{ "outboundWindow4",     HCF_RES_INT, {(void *)RIO_CHANNEL_DOORBELL }},
	{ "outboundWindow5",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow6",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow7",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "outboundWindow8",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow0",     HCF_RES_INT, {(void *) RIO_CHANNEL_RESERVED}},
	{ "inboundWindow1",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow2",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow3",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }},
	{ "inboundWindow4",     HCF_RES_INT, {(void *)RIO_CHANNEL_UNRESERVED }}

};
#define m85xxCPU1Num    NELEMENTS(m85xxCPU1Resources)

const struct hcfResource smEnd0Resources[] = {
    { "regBase",	HCF_RES_INT, {(void *)CCSBAR} },
    { "SM_ANCHOR_OFFSET", HCF_RES_INT, {(void *)SM_ANCHOR_OFFSET}},
    { "SM_MEM_ADRS", HCF_RES_INT,{(void *)SM_MEM_ADRS }},
    { "SM_MEM_SIZE", HCF_RES_INT,{(void *)SM_MEM_SIZE}},
    { "SM_TAS_TYPE", HCF_RES_INT,{(void *)SM_TAS_TYPE} },
    { "SM_INT_TYPE", HCF_RES_INT,{(void *)SM_INT_TYPE} },
    { "SM_INT_ARG1", HCF_RES_INT,{(void *)SM_INT_ARG1}},
    { "SM_INT_ARG2", HCF_RES_INT, {(void *)SM_INT_ARG2}},
    { "SM_INT_ARG3", HCF_RES_INT, {(void *)SM_INT_ARG3}},
    { "SM_MBLK_NUM", HCF_RES_INT, {(void *)600}},
    { "SM_CBLK_NUM", HCF_RES_INT, {(void *)200}}
};
#define smEnd0Num    NELEMENTS(smEnd0Resources)

#endif /* INCLUDE_RAPIDIO_BUS */

#ifdef DRV_RESOURCE_M85XXCCSR
const struct hcfResource m85xxLawBarResources[] = {
    { "regBase",                HCF_RES_INT, {(void *)CCSBAR} },
    { "lawBarNum",              HCF_RES_INT, {(void *)8} },
    { "LAWBAR0",                HCF_RES_STRING, {"reserved"} },
    { "LAWBAR1",                HCF_RES_STRING, {"reserved"} },
    { "LAWBAR2",                HCF_RES_STRING, {"reserved"} },
    { "LAWBAR3",                HCF_RES_STRING, {"m85xxPci"} },
    { "LAWBAR4",                HCF_RES_STRING, {"m85xxPci"} },
    { "LAWBAR5",                HCF_RES_STRING, {"reserved"} },
    { "LAWBAR6",                HCF_RES_STRING, {"reserved"} },
    { "LAWBAR7",                HCF_RES_STRING, {"m85xxRio"} },
    { "LAWAR_ENABLE",           HCF_RES_INT, {(void *)LAWAR_ENABLE } },
    { "LAWBAR_ADRS_SHIFT",      HCF_RES_INT, {(void *)LAWBAR_ADRS_SHIFT } },
};

#define m85xxLawBarNum    NELEMENTS(m85xxLawBarResources)
#endif /* DRV_RESOURCE_M85XXCCSR */

#ifdef DRV_PCIBUS_M85XX
IMPORT STATUS sysPci1AutoconfigInclude(PCI_SYSTEM *, PCI_LOC *, UINT);
IMPORT UCHAR sysPci1AutoconfigIntrAssign(PCI_SYSTEM *, PCI_LOC *, UCHAR);
IMPORT STATUS sysPci3AutoconfigInclude(PCI_SYSTEM *, PCI_LOC *, UINT);
IMPORT UCHAR sysPci3AutoconfigIntrAssign(PCI_SYSTEM *, PCI_LOC *, UCHAR);

pconst struct hcfResource m85xxPci0Resources[] = {
    { "regBase",        HCF_RES_INT,    { (void *)(CCSBAR + 0x8000) } },
    { "mem32Addr",      HCF_RES_ADDR,   { (void *)PCI_MEM_ADRS } },
    { "mem32Size",      HCF_RES_INT,    { (void *)PCI_MEM_SIZE } },
    { "memIo32Addr",    HCF_RES_ADDR,   { (void *)PCI_MEMIO_ADRS } },
    { "memIo32Size",    HCF_RES_INT,    { (void *)PCI_MEMIO_SIZE } },
    { "io32Addr",       HCF_RES_ADDR,   { (void *)PCI_IO_ADRS } },
    { "io32Size",       HCF_RES_INT,    { (void *)PCI_IO_SIZE } },
    { "fbbEnable",      HCF_RES_INT,    { (void *)TRUE } },
    { "cacheSize",      HCF_RES_INT,    { (void *)(_CACHE_ALIGN_SIZE / 4) } },
    { "maxLatAllSet",   HCF_RES_INT,    { (void *)PCI_LAT_TIMER } },
    { "autoIntRouteSet",HCF_RES_INT,    { (void *)TRUE } },
    { "includeFuncSet", HCF_RES_ADDR,   { (void *)sysPci1AutoconfigInclude } },
    { "intAssignFuncSet",HCF_RES_ADDR,  { (void *)sysPci1AutoconfigIntrAssign }},
    /* non vxbPciAutoConfig() values */
    { "pimmrBase",      HCF_RES_ADDR,   { (void *)PCI_BRIDGE_PIMMR_BASE_ADRS }},
    { "lclMemAddr",     HCF_RES_ADDR,   { (void *)LOCAL_MEM_LOCAL_ADRS } },
    { "lclMemMapSize",  HCF_RES_INT,    { (void *)(512 * 1024 * 1024) } },
    { "mstrMemBus",     HCF_RES_ADDR,   { (void *)PCI_MSTR_MEM_BUS } },
    { "pciConfigMechanism",HCF_RES_INT, { (void *)(0)} },
    { "maxBusSet",      HCF_RES_INT,    { (void *)16 } },
    /* LAWBAR details */
    { "singleLawBar",   HCF_RES_INT,    { (void *)(TRUE)} },
    { "singleLawBarSize",HCF_RES_INT,   { (void *)(0x10000000)} },
    { "tgtIf",          HCF_RES_INT,    { (void *)(LAWAR_TGTIF_PCI)} },
    { "autoConfig",     HCF_RES_INT,    { (void *)(TRUE)} }
};

#define m85xxPci0Num NELEMENTS(m85xxPci0Resources)

const struct hcfResource m85xxPci1Resources[] = {
    { VXB_REG_BASE,         HCF_RES_INT,  { (void *)(CCSBAR + 0xA000) } },
    { "mem32Addr",          HCF_RES_ADDR, { (void *)PCI_MEM_ADRS3 } },
    { "mem32Size",          HCF_RES_INT,  { (void *)PCI_MEM_SIZE } },
    { "memIo32Addr",        HCF_RES_ADDR, { (void *)PCI_MEMIO_ADRS3 } },
    { "memIo32Size",        HCF_RES_INT,  { (void *)PCI_MEMIO_SIZE } },
    { "io32Addr",           HCF_RES_ADDR, { (void *)PCI_IO_ADRS3 } },
    { "io32Size",           HCF_RES_INT,  { (void *)PCI_IO_SIZE } },
    { "io16Addr",           HCF_RES_ADDR, { (void *)PCI_IO_ADRS3 } },
    { "io16Size",           HCF_RES_INT,  { (void *)PCI_IO_SIZE } },
    { "fbbEnable",          HCF_RES_INT,  { (void *)TRUE } },
    { "cacheSize",          HCF_RES_INT,  { (void *)(_CACHE_ALIGN_SIZE / 4) } },
    { "maxLatAllSet",       HCF_RES_INT,  { (void *)PCI_LAT_TIMER } },
    { "autoIntRouteSet",    HCF_RES_INT,  { (void *)TRUE } },
    { "includeFuncSet",     HCF_RES_ADDR, { (void *)sysPci3AutoconfigInclude } },
    { "intAssignFuncSet",   HCF_RES_ADDR, { (void *)sysPci3AutoconfigIntrAssign } },
    /* non vxbPciAutoConfig() values */
    { "pimmrBase",          HCF_RES_ADDR, { (void *)PCI_BRIDGE_PIMMR_BASE_ADRS } },
    { "lclMemAddr",         HCF_RES_ADDR, { (void *)LOCAL_MEM_LOCAL_ADRS } },
    { "lclMemMapSize",      HCF_RES_INT,  { (void *)(512 * 1024 * 1024) } },
    { "mstrMemBus",         HCF_RES_ADDR, { (void *)PCI_MSTR_MEM_BUS } },
    { "maxBusSet",          HCF_RES_INT,  { (void *)16 } },
    /* LAWBAR details */
    { "tgtIf",              HCF_RES_INT,  { (void *)(LAWAR_TGTIF_PCIEX)} },
    /* Window Attributes - Defaults to 8540 type if none given */
    { "owAttrMem",          HCF_RES_INT,  { (void *)(PCI_WINDOW_ENABLE_BIT | PCI_OUT_ATTR_RTT_MEM |  PCI_OUT_ATTR_WTT_MEM) } },
    { "owAttrMemIo",        HCF_RES_INT,  { (void *)(PCI_WINDOW_ENABLE_BIT | PCI_OUT_ATTR_RTT_MEM |  PCI_OUT_ATTR_WTT_MEM) } },
    { "owAttrIo",           HCF_RES_INT,  { (void *)(PCI_WINDOW_ENABLE_BIT | PCI_OUT_ATTR_RTT_IO |  PCI_OUT_ATTR_WTT_IO) } },
    { "iwAttr",             HCF_RES_INT,  { (void *)(PCI_WINDOW_ENABLE_BIT | PCI_PREFETCHABLE | PCI_IN_ATTR_RTT_LM_READ_SNOOP | PCI_IN_ATTR_RTT_LM_WRITE_SNOOP | PCI_IN_ATTR_TGI_LM) } },
    { "singleLawBar",       HCF_RES_INT,  { (void *)(TRUE)} },
    { "singleLawBarSize",   HCF_RES_INT,  { (void *)(0x10000000)} },
    { "pciConfigMechanism", HCF_RES_INT,  { (void *)(0)} },
    { "autoConfig",         HCF_RES_INT,  { (void *)(TRUE)} },
    { "pciExpressHost",     HCF_RES_INT,  { (void *)(TRUE)} },
#ifdef INCLUDE_INTCTLR_DYNAMIC_LIB
    { "msiEnable",          HCF_RES_INT,  { (void *)(TRUE)} },
    { "dynamicInterrupts",  HCF_RES_INT,  { (void *)(TRUE)} },
    { "msiDataWorkaround",      HCF_RES_INT,  { (void *)(TRUE)} },
    { "msiDataWorkaroundAddr",  HCF_RES_INT,  { (void *)(0x80000000)} }
#else
    { "msiEnable",          HCF_RES_INT,  { (void *)(FALSE)} },
    { "dynamicInterrupts",  HCF_RES_INT,  { (void *)(FALSE)} },
    { "msiDataWorkaround",      HCF_RES_INT,  { (void *)(FALSE)} },
    { "msiDataWorkaroundAddr",  HCF_RES_INT,  { (void *)(0x80000000)} }
#endif /* INCLUDE_INTCTLR_DYNAMIC_LIB */
};
#define m85xxPci1Num NELEMENTS(m85xxPci1Resources)

#endif /* DRV_PCIBUS_M85XX */

const struct hcfResource ns165500Resources[] = {
    { "regBase",     HCF_RES_INT,  { (void *)(CCSBAR + 0x4500) } },
    { "clkFreq",     HCF_RES_ADDR, { (void *)sysClkFreqGet } },
    { "regInterval", HCF_RES_INT,  { (void *)1 } }
};
#define ns165500Num NELEMENTS(ns165500Resources)

const struct hcfResource ns165501Resources[] = {
    { "regBase",     HCF_RES_INT,  { (void *)(CCSBAR + 0x4600) } },
    { "clkFreq",     HCF_RES_ADDR, { (void *)sysClkFreqGet} },
    { "regInterval", HCF_RES_INT,  { (void *)1 } }
};
#define ns165501Num NELEMENTS(ns165501Resources)

#ifdef PRJ_BUILD
/* 定义串口寄存器以及串口参考时钟 */
#define SERIAL0_BASE_ADRS (LBC_CS2_LOCAL_ADRS)
#define SERIAL1_BASE_ADRS (LBC_CS4_LOCAL_ADRS)
#define SERIAL2_BASE_ADRS (LBC_CS5_LOCAL_ADRS)
#define SERIAL3_BASE_ADRS (LBC_CS6_LOCAL_ADRS)

#define UART_FREQ (14745600)
#define SERIAL0_IRQ  (EPIC_VEC_EXT_IRQ0 + 1)
#define SERIAL1_IRQ	 (EPIC_VEC_EXT_IRQ0 + 2)
#define SERIAL2_IRQ	 (EPIC_VEC_EXT_IRQ0 + 3)
#define SERIAL3_IRQ	 (EPIC_VEC_EXT_IRQ0 + 4)
#define DUART_REG_ADDR_INTERVAL 1                     /* duart vector register distance */


static unsigned int rs422Division(unsigned int xtal, unsigned int baud)
{
    return (xtal / (16 * baud));
}

const struct hcfResource serial0Resources[] = {
    { VXB_REG_BASE,  HCF_RES_INT,  {(void *)SERIAL0_BASE_ADRS} },
    { "irq",         HCF_RES_INT,  {(void *)SERIAL0_IRQ} },
    { "regInterval", HCF_RES_INT,  {(void *)DUART_REG_ADDR_INTERVAL} },
    { "irqLevel",    HCF_RES_INT,  {(void *)SERIAL0_IRQ} },
    { "clkFreq",	 HCF_RES_INT,  {(void *)UART_FREQ} },
    /* { "divisorCalc", HCF_RES_ADDR, {(void *)rs422Division}} */
};
#define serial0Num NELEMENTS(serial0Resources)

const struct hcfResource serial1Resources[] = {
    { VXB_REG_BASE,  HCF_RES_INT,  {(void *)SERIAL1_BASE_ADRS} },
    { "irq",         HCF_RES_INT,  {(void *)SERIAL1_IRQ} },
    { "regInterval", HCF_RES_INT,  {(void *)DUART_REG_ADDR_INTERVAL} },
    { "irqLevel",    HCF_RES_INT,  {(void *)SERIAL1_IRQ} },
    { "clkFreq",	 HCF_RES_INT,  {(void *)UART_FREQ} },
    /* { "divisorCalc", HCF_RES_ADDR, {(void *)rs422Division}} */
};
#define serial1Num NELEMENTS(serial1Resources)

const struct hcfResource serial2Resources[] = {
    { VXB_REG_BASE,  HCF_RES_INT,  {(void *)SERIAL2_BASE_ADRS} },
    { "irq",         HCF_RES_INT,  {(void *)SERIAL2_IRQ} },
    { "regInterval", HCF_RES_INT,  {(void *)DUART_REG_ADDR_INTERVAL} },
    { "irqLevel",    HCF_RES_INT,  {(void *)SERIAL2_IRQ} },
    { "clkFreq",	 HCF_RES_INT,  {(void *)UART_FREQ} },
    /* { "divisorCalc", HCF_RES_ADDR, {(void *)rs422Division}} */
};
#define serial2Num NELEMENTS(serial2Resources)
const struct hcfResource serial3Resources[] = {
    { VXB_REG_BASE,  HCF_RES_INT,  {(void *)SERIAL3_BASE_ADRS} },
    { "irq",         HCF_RES_INT,  {(void *)SERIAL3_IRQ} },
    { "regInterval", HCF_RES_INT,  {(void *)DUART_REG_ADDR_INTERVAL} },
    { "irqLevel",    HCF_RES_INT,  {(void *)SERIAL3_IRQ} },
    { "clkFreq",	 HCF_RES_INT,  {(void *)UART_FREQ} },
    /* { "divisorCalc", HCF_RES_ADDR, {(void *)rs422Division}} */
};
#define serial3Num NELEMENTS(serial3Resources)
#endif	/* PRJ_BUILD */

const struct intrCtlrInputs ppcIntCtlrInputs[] = {
    { 0, "epic", 0, 0 },
    { 1, "m85xxTimerDev", 0, 0 },
    { 3, "m85xxTimerDev", 0, 2 },
    { 4, "m85xxTimerDev", 0, 1 },
};

const struct hcfResource ppcIntCtlr0Resources[] = {
    { "regBase",        HCF_RES_INT,  { (void *)TRUE } },
    { "input",		    HCF_RES_ADDR, { (void *)&ppcIntCtlrInputs[0] } },
    { "inputTableSize", HCF_RES_INT,  { (void *)NELEMENTS(ppcIntCtlrInputs) } },
};
#define ppcIntCtlr0Num NELEMENTS(ppcIntCtlr0Resources)

/* 单个中断的极性，触发类型定义 */
#ifdef PRJ_BUILD
/* 此处宏是定义中断极性触发的东西 */
#ifndef VXB_INTR_TRIG_ACTIVE_LOW
#define VXB_INTR_TRIG_FALLING_EDGE  (VXB_INTR_TRIG_NEG | VXB_INTR_TRIG_EDGE)
#define VXB_INTR_TRIG_RISING_EDGE   (VXB_INTR_TRIG_POS | VXB_INTR_TRIG_EDGE)
#define VXB_INTR_TRIG_ACTIVE_LOW    (VXB_INTR_TRIG_NEG | VXB_INTR_TRIG_LEVEL)
#define VXB_INTR_TRIG_ACTIVE_HIGH   (VXB_INTR_TRIG_POS | VXB_INTR_TRIG_LEVEL)
#endif

struct intrCtlrTrigger epicIntCtlrTrigger[]=
{
	{EPIC_VEC_EXT_IRQ0+0,VXB_INTR_TRIG_EDGE|VXB_INTR_TRIG_NEG},
	/* 看到逻辑里面的电平是反的,默认为低电平,此项目中需要将电平触发方式改为高电平的形式 */
	{EPIC_VEC_EXT_IRQ0+1,VXB_INTR_TRIG_ACTIVE_HIGH},
	{EPIC_VEC_EXT_IRQ0+2,VXB_INTR_TRIG_ACTIVE_HIGH},
	{EPIC_VEC_EXT_IRQ0+3,VXB_INTR_TRIG_ACTIVE_HIGH},
	{EPIC_VEC_EXT_IRQ0+4,VXB_INTR_TRIG_ACTIVE_HIGH},
	/* {EPIC_VEC_EXT_IRQ0+1,VXB_INTR_TRIG_EDGE|VXB_INTR_TRIG_NEG}, */
	/* {EPIC_VEC_EXT_IRQ0+2,VXB_INTR_TRIG_EDGE|VXB_INTR_TRIG_NEG}, */
	/* {EPIC_VEC_EXT_IRQ0+3,VXB_INTR_TRIG_EDGE|VXB_INTR_TRIG_NEG}, */
	/* {EPIC_VEC_EXT_IRQ0+4,VXB_INTR_TRIG_EDGE|VXB_INTR_TRIG_NEG}, */
	/* {EPIC_VEC_EXT_IRQ0+5,VXB_INTR_TRIG_EDGE|VXB_INTR_TRIG_NEG}, */
};
#endif	/* PRJ_BUILD */

struct intrCtlrInputs epicInputs[] = {
    { EPIC_DUART_INT_VEC,    "ns16550", 0, 0 },
    { EPIC_DUART_INT_VEC,    "ns16550", 1, 0 },
    { EPIC_TSEC1TX_INT_VEC,  XTSEC_NAME, 0, 0 },
    { EPIC_TSEC1RX_INT_VEC,  XTSEC_NAME, 0, 1 },
    { EPIC_TSEC1ERR_INT_VEC, XTSEC_NAME, 0, 2 },
    { EPIC_TSEC2TX_INT_VEC,  XTSEC_NAME, 1, 0 },
    { EPIC_TSEC2RX_INT_VEC,  XTSEC_NAME, 1, 1 },
    { EPIC_TSEC2ERR_INT_VEC, XTSEC_NAME, 1, 2 },
    { EPIC_TSEC3TX_INT_VEC,  XTSEC_NAME, 2, 0 },
    { EPIC_TSEC3RX_INT_VEC,  XTSEC_NAME, 2, 1 },
    { EPIC_TSEC3ERR_INT_VEC, XTSEC_NAME, 2, 2 },
    { EPIC_TSEC4TX_INT_VEC,  XTSEC_NAME, 3, 0 },
    { EPIC_TSEC4RX_INT_VEC,  XTSEC_NAME, 3, 1 },
    { EPIC_TSEC4ERR_INT_VEC, XTSEC_NAME, 3, 2 },
#ifdef PRJ_BUILD				/* 添加外部中断的定义 */
    {EPIC_VEC_EXT_IRQ0+0,     "irq0",0,0},
    {EPIC_VEC_EXT_IRQ0+1,     "irq1",1,0},
    {EPIC_VEC_EXT_IRQ0+2,     "irq2",2,0},
    {EPIC_VEC_EXT_IRQ0+3,     "irq3",3,0},
    {EPIC_VEC_EXT_IRQ0+4,     "irq4",4,0},
    {EPIC_VEC_EXT_IRQ0+5,     "irq5",5,0},
#endif
};

const struct hcfResource epic0Resources[] = {
    { "regBase",        HCF_RES_INT,  { (void *)CCSBAR } },
    { "input",          HCF_RES_ADDR, { (void *)&epicInputs[0] } },
    { "inputTableSize", HCF_RES_INT,  { (void *)NELEMENTS(epicInputs) } },
    { "numCpus",        HCF_RES_INT,  { (void *)1 } },
#ifdef PRJ_BUILD
    { "trigger",            HCF_RES_ADDR,   { (void *)&epicIntCtlrTrigger[0] } },
    { "triggerTableSize",	HCF_RES_INT,	{ (void *)NELEMENTS(epicIntCtlrTrigger) }},
	{ "exPolar",        HCF_RES_INT,  { (void *)EPIC_INT_ACT_LOW } }, /* 外部中断的默认极性，如果不定义，默认为高 */
    { "exSense",            HCF_RES_INT,    { (void *)EPIC_SENSE_LVL } }, /* 外部中断的默认触发类型，如果不定义，默认为电平触发 */
    { "inPolar",        HCF_RES_INT,  { (void *)EPIC_INT_ACT_HIGH } }, /* 外部中断的默认极性，如果不定义，默认为高 */
    { "inSense",            HCF_RES_INT,    { (void *)EPIC_SENSE_LVL } }, /* 外部中断的默认触发类型，如果不定义，默认为电平触发 */
    { "crtEnable",            HCF_RES_INT,    { (void *)FALSE } }, /* 是否使能critical interrupt，不定为默认关闭 */
        /** todo exIntWkrd是做什么的? */
    { "exIntWkrd",            HCF_RES_INT,    { (void *)FALSE } }, /* IRQ workaround should be used (default FALSE).不知道什么作用 */
    { "numInts",            HCF_RES_INT,    { (void *)255 } }, /* 中断表的个数，不定义为默认255 */
#endif	/* PRJ_BUILD */

#ifdef DRV_PCIBUS_M85XX
    { "exPolar",        HCF_RES_INT,  { (void *)EPIC_INT_ACT_LOW } },
#endif
};
#define epic0Num NELEMENTS(epic0Resources)

struct hcfResource m85xxTimerResources[] =  {
    { "regBase",       HCF_RES_INT,  { (void *)0 } },
    { "decMinClkRate", HCF_RES_INT,  { (void *)SYS_CLK_RATE_MIN } },
    { "decMaxClkRate", HCF_RES_INT,  { (void *)SYS_CLK_RATE_MAX } },
    { "fitMinClkRate", HCF_RES_INT,  { (void *)AUX_CLK_RATE_MIN } },
    { "fitMaxClkRate", HCF_RES_INT,  { (void *)AUX_CLK_RATE_MAX } },
    { "sysClockFreq",  HCF_RES_ADDR, { (void *)&sysClkFreqGet } }
};
#define m85xxTimerNum   NELEMENTS(m85xxTimerResources)


const struct hcfDevice hcfDeviceList[] = {
    { "ppcIntCtlr", 0, VXB_BUSID_PLB, 0, ppcIntCtlr0Num, ppcIntCtlr0Resources },
    { "epic", 0, VXB_BUSID_PLB, 0, epic0Num, epic0Resources },

    { "m85xxTimerDev", 0, VXB_BUSID_PLB, 0, m85xxTimerNum, m85xxTimerResources},

    { "ns16550", 0, VXB_BUSID_PLB, 0, ns165500Num, ns165500Resources },
#ifdef PRJ_BUILD
    { "ns16550", 1, VXB_BUSID_PLB, 0, ns165501Num, ns165501Resources },
    { "ns16550", 2, VXB_BUSID_PLB, 0, serial0Num, serial0Resources},
    { "ns16550", 3, VXB_BUSID_PLB, 0, serial1Num, serial1Resources},
    { "ns16550", 4, VXB_BUSID_PLB, 0, serial2Num, serial2Resources},
    { "ns16550", 5, VXB_BUSID_PLB, 0, serial3Num, serial3Resources},
#endif	/* PRJ_BUILD */

#ifdef INCLUDE_MV88E1X11PHY
    { "mv88E14xPhy", 0,  VXB_BUSID_PLB,  0,  NELEMENTS(mv88e1x11fixup0),  mv88e1x11fixup0},
    { "mv88E14xPhy", 1,  VXB_BUSID_PLB,  0,  NELEMENTS(mv88e1x11fixup1),  mv88e1x11fixup1},
    { "mv88E14xPhy", 2,  VXB_BUSID_PLB,  0,  NELEMENTS(mv88e1x11fixup2),  mv88e1x11fixup2},
    { "mv88E14xPhy", 3,  VXB_BUSID_PLB,  0,  NELEMENTS(mv88e1x11fixup3),  mv88e1x11fixup3},
#endif
	
    { XTSEC_NAME, 0, VXB_BUSID_PLB, 0, tsecVxbEnd0Num, tsecVxbEnd0Resources },
		/* 在vxworks中添加多余的网口 */
    { XTSEC_NAME, 1, VXB_BUSID_PLB, 0, tsecVxbEnd1Num, tsecVxbEnd1Resources },
    { XTSEC_NAME, 2, VXB_BUSID_PLB, 0, tsecVxbEnd2Num, tsecVxbEnd2Resources },
    { XTSEC_NAME, 3, VXB_BUSID_PLB, 0, tsecVxbEnd3Num, tsecVxbEnd3Resources },

#ifdef INCLUDE_RAPIDIO_BUS
    { "m85xxRio", 0, VXB_BUSID_PLB, 0, m85xxRio0Num, m85xxRio0Resources },
    { "m85xxCPU", 0, VXB_BUSID_RAPIDIO, 0, m85xxCPU0Num, m85xxCPU0Resources },
    { "m85xxCPU", 1, VXB_BUSID_RAPIDIO, 0, m85xxCPU1Num, m85xxCPU1Resources },
    { "smEnd", 0, VXB_BUSID_PLB,0, smEnd0Num, smEnd0Resources },
#endif /* INCLUDE_RAPIDIO_BUS */
#ifdef DRV_PCIBUS_M85XX
    { "m85xxPci", 0, VXB_BUSID_PLB, 0, m85xxPci0Num, m85xxPci0Resources },
    { "m85xxPci", 1, VXB_BUSID_PLB, 0, m85xxPci1Num, m85xxPci1Resources },
#endif /* DRV_PCIBUS_M85XX */

#ifdef DRV_RESOURCE_M85XXCCSR
    { "m85xxCCSR", 0, VXB_BUSID_PLB, 0, m85xxLawBarNum, m85xxLawBarResources },
#endif /* DRV_RESOURCE_M85XXCCSR */
};
const int hcfDeviceNum = NELEMENTS(hcfDeviceList);

VXB_INST_PARAM_OVERRIDE sysInstParamTable[] =
    {
    { NULL, 0, NULL, VXB_PARAM_END_OF_LIST, {(void *)0} }
    };
