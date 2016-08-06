# Makefile generated by the project manager
#


# GENERATED: Sat Aug 06 09:26:39 +0900 2016
# DO NOT EDIT - file is regenerated whenever the project changes


## core information

ifeq ($(wildcard Makefile.mk),)
PRJ_DIR        = ..
else
PRJ_DIR        = .
endif
PRJ_FILE       = sbc8548m-s9_vx.wpj
PRJ_FULL_PATH  = $(WIND_HOME)/sbc8548-t9/sbc8548m-s9_vx
PRJ_TYPE       = vxWorks
PRJ_OBJS       = sysALib.o sysLib.o usrAppInit.o usrRtpAppInit.o prjConfig.o linkSyms.o test_file.o test_uart.o
PRJ_MAKEFILE   = $(PRJ_DIR)/Makefile.mk
BOOT_OBJS      = romInit.o romStart.o $(BOOT_EXTRA)
BUILD_SPEC     = default
BSP_DIR        = $(WIND_HOME)/sbc8548-t9/SB8548M-s9_boot/wrSbc8548
VSB_DIR        = $(TGT_DIR)/lib
TGT_DIR        = $(WIND_BASE)/target



## set searching directories for dependencies

vpath %.c $(BSP_DIR)
vpath %.cpp $(BSP_DIR)
vpath %.cxx $(BSP_DIR)
vpath %.s $(BSP_DIR)


## build-configuration info

ifeq ($(BUILD_SPEC),default)
CPU            = PPC32
TOOL           = e500v2gnu
TOOL_FAMILY    = gnu
DEFAULT_RULE   = vxWorks
endif

ifeq ($(BUILD_SPEC),default_rom)
CPU            = PPC32
TOOL           = e500v2gnu
TOOL_FAMILY    = gnu
DEFAULT_RULE   = vxWorks_rom
endif

ifeq ($(BUILD_SPEC),default_romCompress)
CPU            = PPC32
TOOL           = e500v2gnu
TOOL_FAMILY    = gnu
DEFAULT_RULE   = vxWorks_romCompress
endif

ifeq ($(BUILD_SPEC),default_romResident)
CPU            = PPC32
TOOL           = e500v2gnu
TOOL_FAMILY    = gnu
DEFAULT_RULE   = vxWorks_romResident
endif


## Memory adresses variables

RAM_LOW_ADRS = 00100000
RAM_HIGH_ADRS = 01000000
LOCAL_MEM_LOCAL_ADRS = 00000000
DATA_SEG_ALIGN = 00040000

## component-configuration info

COMPONENTS = DRV_INTCTLR_EPIC \
	DRV_INTCTLR_PPC \
	DRV_SIO_NS16550 \
	DRV_TIMER_M85XX \
	INCLUDE_AIM_CACHE \
	INCLUDE_AIM_MMU_BASIC \
	INCLUDE_AIM_MMU_CONFIG \
	INCLUDE_AIM_MMU_MEM_POOL_CONFIG \
	INCLUDE_AIM_MMU_PT_PROTECTION \
	INCLUDE_AIM_MMU_SHOW \
	INCLUDE_ANSI_ABORT \
	INCLUDE_ANSI_ABS \
	INCLUDE_ANSI_ASSERT \
	INCLUDE_ANSI_ATOF \
	INCLUDE_ANSI_BSEARCH \
	INCLUDE_ANSI_CTYPE \
	INCLUDE_ANSI_LOCALE \
	INCLUDE_ANSI_LONGJMP \
	INCLUDE_ANSI_MATH \
	INCLUDE_ANSI_MEMCHR \
	INCLUDE_ANSI_MEMCMP \
	INCLUDE_ANSI_MEMCPY \
	INCLUDE_ANSI_MEMMOVE \
	INCLUDE_ANSI_MEMSET \
	INCLUDE_ANSI_STDIO \
	INCLUDE_ANSI_STDIO_EXTRA \
	INCLUDE_ANSI_STDLIB \
	INCLUDE_ANSI_STDLIB_NUMBERS \
	INCLUDE_ANSI_STRCASECMP \
	INCLUDE_ANSI_STRCAT \
	INCLUDE_ANSI_STRCMP \
	INCLUDE_ANSI_STRCPY \
	INCLUDE_ANSI_STRDUP \
	INCLUDE_ANSI_STRERROR \
	INCLUDE_ANSI_STRING \
	INCLUDE_ANSI_STRLCPY \
	INCLUDE_ANSI_STRLEN \
	INCLUDE_ANSI_STRNCASECMP \
	INCLUDE_ANSI_STRNCAT \
	INCLUDE_ANSI_STRNCMP \
	INCLUDE_ANSI_STRNCPY \
	INCLUDE_ANSI_STRTOD \
	INCLUDE_ANSI_TIME \
	INCLUDE_APPL_LOG_UTIL \
	INCLUDE_ARP_API \
	INCLUDE_ATOMIC_OPERATORS \
	INCLUDE_AUX_CLK \
	INCLUDE_AVL_UINT_TREES \
	INCLUDE_BASE_KERNEL \
	INCLUDE_BLIB \
	INCLUDE_BLIB_ARCH \
	INCLUDE_BOOT_LINE_INIT \
	INCLUDE_BSP_MACROS \
	INCLUDE_CACHE_ENABLE \
	INCLUDE_CACHE_SUPPORT \
	INCLUDE_CLASS_LIB \
	INCLUDE_CLASS_LIST \
	INCLUDE_COMMON_NET \
	INCLUDE_COPROCESSOR \
	INCLUDE_COPROCESSOR_SHOW \
	INCLUDE_CPU_LIGHT_PWR_MGR \
	INCLUDE_CPU_PWR_MGMT \
	INCLUDE_CTORS_DTORS \
	INCLUDE_DEBUG \
	INCLUDE_DEBUG_SHELL_CMD \
	INCLUDE_DEFERRED_INTERRUPT_ENABLE \
	INCLUDE_DEVICE_MANAGER \
	INCLUDE_DISK_UTIL \
	INCLUDE_DISK_UTIL_SHELL_CMD \
	INCLUDE_DLL \
	INCLUDE_DMA_SYS \
	INCLUDE_DOSFS \
	INCLUDE_DOSFS_CACHE \
	INCLUDE_DOSFS_CHKDSK \
	INCLUDE_DOSFS_DIR_FIXED \
	INCLUDE_DOSFS_DIR_VFAT \
	INCLUDE_DOSFS_FAT \
	INCLUDE_DOSFS_FMT \
	INCLUDE_DOSFS_MAIN \
	INCLUDE_DOSFS_PRTMSG_LEVEL \
	INCLUDE_DOSFS_SHOW \
	INCLUDE_DOSFS_VOL_BIO_BUFFER_SIZE \
	INCLUDE_EDR_STUB \
	INCLUDE_EDR_SYSDBG_FLAG \
	INCLUDE_END \
	INCLUDE_END_COMMON \
	INCLUDE_END_ETHER_HDR \
	INCLUDE_ENV_VARS \
	INCLUDE_ERF \
	INCLUDE_ETSEC_VXB_END \
	INCLUDE_EVENTPOINT_STUB \
	INCLUDE_EXC_HANDLING \
	INCLUDE_EXC_SHOW \
	INCLUDE_EXC_TASK \
	INCLUDE_FLOATING_POINT \
	INCLUDE_FORMATTED_IO \
	INCLUDE_FORMATTED_OUT_BASIC \
	INCLUDE_FP_TYPE \
	INCLUDE_FS_EVENT_UTIL \
	INCLUDE_FS_MONITOR \
	INCLUDE_FTP \
	INCLUDE_GENERICPHY \
	INCLUDE_GENERICTBIPHY \
	INCLUDE_GETOPT \
	INCLUDE_GNU_INTRINSICS \
	INCLUDE_HANDLE_SHOW \
	INCLUDE_HASH \
	INCLUDE_HISTORY_FILE_SHELL_CMD \
	INCLUDE_HOOKS \
	INCLUDE_HOOK_SHOW \
	INCLUDE_HOST_TBL \
	INCLUDE_HWMEM_ALLOC \
	INCLUDE_IFCONFIG \
	INCLUDE_INTCTLR_LIB \
	INCLUDE_IO_BASIC \
	INCLUDE_IO_FILE_SYSTEM \
	INCLUDE_IO_MISC \
	INCLUDE_IO_POSIX \
	INCLUDE_IO_REMOVABLE \
	INCLUDE_IO_RTP \
	INCLUDE_IO_SYSTEM \
	INCLUDE_IPAIP \
	INCLUDE_IPAIP_GLOBAL_CONFIGS \
	INCLUDE_IPARP_CMD \
	INCLUDE_IPATTACH \
	INCLUDE_IPCOM \
	INCLUDE_IPCOM_SHELL_CMD \
	INCLUDE_IPCOM_USE_ETHERNET \
	INCLUDE_IPCOM_USE_INET \
	INCLUDE_IPFTPC \
	INCLUDE_IPFTPS \
	INCLUDE_IPFTP_CMD \
	INCLUDE_IPIFCONFIG_CMD \
	INCLUDE_IPNET \
	INCLUDE_IPNETSTAT_CMD \
	INCLUDE_IPNET_IFCONFIG_1 \
	INCLUDE_IPNET_LOOPBACK_CONFIG \
	INCLUDE_IPNET_SOCKET \
	INCLUDE_IPNET_STACK \
	INCLUDE_IPNET_USE_LOOPBACK \
	INCLUDE_IPNET_USE_ROUTESOCK \
	INCLUDE_IPNET_USE_SOCK_COMPAT \
	INCLUDE_IPPING_CMD \
	INCLUDE_IPPROXYARP \
	INCLUDE_IPTCP \
	INCLUDE_IPV4 \
	INCLUDE_IPWRAP_ARP \
	INCLUDE_IPWRAP_IFCONFIG \
	INCLUDE_IPWRAP_INETLIB \
	INCLUDE_IPWRAP_IPPROTO \
	INCLUDE_IPWRAP_NETSTAT \
	INCLUDE_IPWRAP_PING \
	INCLUDE_ISR_DEFER \
	INCLUDE_JOB_QUEUE \
	INCLUDE_JOB_TASK \
	INCLUDE_KERNEL \
	INCLUDE_KERNEL_ARCH \
	INCLUDE_LINKBUFPOOL \
	INCLUDE_LOADER \
	INCLUDE_LOADER_CODE \
	INCLUDE_LOCK_TEXT_SECTION \
	INCLUDE_LOGGING \
	INCLUDE_LSTLIB \
	INCLUDE_MBUF_UTIL1 \
	INCLUDE_MEMORY_CONFIG \
	INCLUDE_MEM_MGR_BASIC \
	INCLUDE_MEM_MGR_FULL \
	INCLUDE_MEM_MGR_INFO \
	INCLUDE_MEM_SHOW \
	INCLUDE_MII_BUS \
	INCLUDE_MMU_BASIC \
	INCLUDE_MMU_GLOBAL_MAP \
	INCLUDE_MMU_OPTIMIZE \
	INCLUDE_MODULE_MANAGER \
	INCLUDE_MODULE_SHELL_CMD \
	INCLUDE_MSG_Q \
	INCLUDE_MSG_Q_CREATE_DELETE \
	INCLUDE_MSG_Q_EVENTS \
	INCLUDE_MSG_Q_INFO \
	INCLUDE_MUX \
	INCLUDE_MUX2 \
	INCLUDE_MUX2_OVER_END \
	INCLUDE_MUXTK \
	INCLUDE_MUXTK_OVER_END \
	INCLUDE_MUX_COMMON \
	INCLUDE_MV88E1X11PHY \
	INCLUDE_NBIO_LOG \
	INCLUDE_NETBUFLIB \
	INCLUDE_NETBUFPOOL \
	INCLUDE_NETMASK_GET \
	INCLUDE_NETSTAT \
	INCLUDE_NETWORK \
	INCLUDE_NET_BOOT \
	INCLUDE_NET_DAEMON \
	INCLUDE_NET_DRV \
	INCLUDE_NET_HOST_SETUP \
	INCLUDE_NET_REM_IO \
	INCLUDE_OBJ_INFO \
	INCLUDE_OBJ_LIB \
	INCLUDE_OBJ_LIB_ARCH \
	INCLUDE_OFFSET_SYMBOLS \
	INCLUDE_PARAM_SYS \
	INCLUDE_PING \
	INCLUDE_PIPES \
	INCLUDE_PLB_BUS \
	INCLUDE_POOL \
	INCLUDE_POSIX_CLOCKS \
	INCLUDE_POSIX_DIRLIB \
	INCLUDE_POSIX_FS \
	INCLUDE_PTYDRV \
	INCLUDE_Q_FIFO \
	INCLUDE_Q_LIB \
	INCLUDE_Q_PRI_BMAP \
	INCLUDE_Q_PRI_DELTA \
	INCLUDE_Q_PRI_LIST \
	INCLUDE_RAWFS \
	INCLUDE_REBOOT_HOOKS \
	INCLUDE_REMLIB \
	INCLUDE_RNG_BUF \
	INCLUDE_SELECT \
	INCLUDE_SELECT_SUPPORT \
	INCLUDE_SEM_BINARY \
	INCLUDE_SEM_BINARY_CREATE \
	INCLUDE_SEM_COUNTING \
	INCLUDE_SEM_COUNTING_CREATE \
	INCLUDE_SEM_DELETE \
	INCLUDE_SEM_EVENTS \
	INCLUDE_SEM_INFO \
	INCLUDE_SEM_LIB \
	INCLUDE_SEM_MUTEX \
	INCLUDE_SEM_MUTEX_CREATE \
	INCLUDE_SEM_READ_WRITE \
	INCLUDE_SEM_READ_WRITE_CREATE \
	INCLUDE_SHELL \
	INCLUDE_SHELL_BANNER \
	INCLUDE_SHELL_CORE \
	INCLUDE_SHELL_HISTORY_FILE \
	INCLUDE_SHELL_INTERP_C \
	INCLUDE_SHELL_INTERP_CMD \
	INCLUDE_SHELL_VI_MODE \
	INCLUDE_SIGNALS \
	INCLUDE_SIGNAL_ARCH \
	INCLUDE_SIO \
	INCLUDE_SIO_UTILS \
	INCLUDE_SLL \
	INCLUDE_SOCKLIB \
	INCLUDE_SPE \
	INCLUDE_SPINLOCK \
	INCLUDE_SPY \
	INCLUDE_STANDALONE_SYM_TBL \
	INCLUDE_STAT_SYM_TBL \
	INCLUDE_STDIO \
	INCLUDE_STRING_SEARCH \
	INCLUDE_SYM_SHELL_CMD \
	INCLUDE_SYM_TBL \
	INCLUDE_SYM_TBL_INIT \
	INCLUDE_SYM_TBL_SHOW \
	INCLUDE_SYSCLK_INIT \
	INCLUDE_SYSHW_INIT \
	INCLUDE_SYSHW_INIT2 \
	INCLUDE_SYS_START \
	INCLUDE_TASK_CREATE_DELETE \
	INCLUDE_TASK_CREATE_HOOKS \
	INCLUDE_TASK_HOOKS \
	INCLUDE_TASK_HOOKS_SHOW \
	INCLUDE_TASK_INFO \
	INCLUDE_TASK_LIST \
	INCLUDE_TASK_RESTART \
	INCLUDE_TASK_SHELL_CMD \
	INCLUDE_TASK_SHOW \
	INCLUDE_TASK_SWAP_HOOKS \
	INCLUDE_TASK_SWITCH_HOOKS \
	INCLUDE_TASK_VARS \
	INCLUDE_TFFS \
	INCLUDE_TFFS_MOUNT \
	INCLUDE_TIMER_STUB \
	INCLUDE_TIMER_SYS \
	INCLUDE_TIMER_SYS_DELAY \
	INCLUDE_TIMESTAMP \
	INCLUDE_TIMEX \
	INCLUDE_TIP \
	INCLUDE_TL_FTL \
	INCLUDE_TSEC_MDIO \
	INCLUDE_TTY_DEV \
	INCLUDE_TYLIB \
	INCLUDE_UNLOADER \
	INCLUDE_UNLOADER_SHELL_CMD \
	INCLUDE_USER_APPL \
	INCLUDE_USE_NATIVE_SHELL \
	INCLUDE_VXBUS \
	INCLUDE_VXBUS_ACTIVATE \
	INCLUDE_VXBUS_DEVICE_CONNECT \
	INCLUDE_VXBUS_DEV_INIT \
	INCLUDE_VXBUS_INIT \
	INCLUDE_VXBUS_SHOW \
	INCLUDE_VXB_AUX_CLK \
	INCLUDE_VXB_SYS_CLK \
	INCLUDE_VXB_TIMESTAMP \
	INCLUDE_VXEVENTS \
	INCLUDE_VXMEMPROBE_INIT \
	INCLUDE_VXMUX_MBLK \
	INCLUDE_VXMUX_NULLBUFPOOL \
	INCLUDE_VX_TRADITIONAL_SCHEDULER \
	INCLUDE_WATCHDOGS \
	INCLUDE_WATCHDOGS_CREATE_DELETE \
	INCLUDE_WB_RUNTIME_SUPPORT \
	INCLUDE_WDB \
	INCLUDE_WDB_BANNER \
	INCLUDE_WDB_BP \
	INCLUDE_WDB_BP_SYNC \
	INCLUDE_WDB_COMM_END \
	INCLUDE_WDB_CTXT \
	INCLUDE_WDB_DIRECT_CALL \
	INCLUDE_WDB_EVENTPOINTS \
	INCLUDE_WDB_EVENTS \
	INCLUDE_WDB_EXC_NOTIFY \
	INCLUDE_WDB_EXIT_NOTIFY \
	INCLUDE_WDB_FUNC_CALL \
	INCLUDE_WDB_GOPHER \
	INCLUDE_WDB_MDL_SYM_SYNC \
	INCLUDE_WDB_MEM \
	INCLUDE_WDB_POST_KERNEL_INIT \
	INCLUDE_WDB_REG \
	INCLUDE_WDB_START_NOTIFY \
	INCLUDE_WDB_SYS \
	INCLUDE_WDB_SYS_REG \
	INCLUDE_WDB_TASK \
	INCLUDE_WDB_TASK_BP \
	INCLUDE_WDB_TASK_HOOKS \
	INCLUDE_WDB_TASK_REG \
	INCLUDE_WDB_USER_EVENT \
	INCLUDE_WDB_VIO \
	INCLUDE_WDB_VIO_LIB \
	INCLUDE_XBD \
	INIT_HWMEMPOOL_GLOBAL

MANAGED_LIBS = libar521x.a libar521xVxbEnd.a libbcm43xx.a libbcm43xxVxbEnd.a libspl.a libtipc.a libvxdot1x.a libwlan.a libwlanVxbEnd.a
COMPONENT_LIBS = libspl.a

PROJECT_LINK_SYMDEFS = 


include $(TGT_DIR)/h/make/defs.project


## build-configuration info

ifeq ($(BUILD_SPEC),default)
AR               = arppc
AS               = ccppc
BOOT_EXTRA       = 
CC               = ccppc
CC_ARCH_SPEC     = -te500v2
CFLAGS           = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -Wall -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS        = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS_PROJECT = -g -te500v2 -ansi -fno-builtin -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CFLAGS_PROJECT   = -g -te500v2 -ansi -fno-builtin -Wall -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CONFIG_ALL       = ./all
CPP              = ccppc -E -P
CPU_VARIANT      = _ppc85XX_e500v2
EXTRA_MODULES    = $(SUB_TARGETS)
LD               = ldppc
LD_LINK_PATH     = -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_standard -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2common_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2gnu -L$(VSB_DIR)/ppc/PPC32/e500v2common -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common
LD_PARTIAL       = ccppc -r -nostdlib -Wl,-X
LD_PARTIAL_FLAGS = -X -r
LIBS             = $(VX_OS_LIBS)
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_DEPEND    = -M -w
OPTION_GENERATE_DEPENDENCY_FILE = -MD
OPTION_INCLUDE_DIR = -I
OPTION_LANG_C    = -xc
OPTION_UNDEFINE_MACRO = -U
ROM_LINK_ADRS    = fff00000
ROM_WARM_ADRS    = fff00108 # ROM warm entry address
SIZE             = sizeppc
TOOL_FAMILY      = gnu
USR_ENTRY        = usrInit
VMA_START        = 0x0
POST_BUILD_RULE  = 
POST_HEX_BUILD_RULE = 
endif

ifeq ($(BUILD_SPEC),default_rom)
AR               = arppc
AS               = ccppc
BOOT_EXTRA       = 
CC               = ccppc
CC_ARCH_SPEC     = -te500v2
CFLAGS           = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -Wall -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS        = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS_PROJECT = -g -te500v2 -ansi -fno-builtin -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CFLAGS_PROJECT   = -g -te500v2 -ansi -fno-builtin -Wall -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CONFIG_ALL       = ./all
CPP              = ccppc -E -P
CPU_VARIANT      = _ppc85XX_e500v2
EXTRA_MODULES    = $(SUB_TARGETS)
HEX_FLAGS        = 
LD               = ldppc
LD_LINK_PATH     = -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_standard -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2common_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2gnu -L$(VSB_DIR)/ppc/PPC32/e500v2common -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common
LD_PARTIAL       = ccppc -r -nostdlib -Wl,-X
LD_PARTIAL_FLAGS = -X -r
LIBS             = $(VX_OS_LIBS)
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_DEPEND    = -M -w
OPTION_GENERATE_DEPENDENCY_FILE = -MD
OPTION_INCLUDE_DIR = -I
OPTION_LANG_C    = -xc
OPTION_UNDEFINE_MACRO = -U
ROM_LINK_ADRS    = fff00000
ROM_SIZE         = 00100000 # number of bytes of ROM space
ROM_TEXT_ADRS    = fff00100 # ROM entry address
ROM_WARM_ADRS    = fff00108 # ROM warm entry address
SIZE             = sizeppc
TOOL_FAMILY      = gnu
USR_ENTRY        = usrInit
VMA_START        = 0x0
POST_BUILD_RULE  = 
POST_HEX_BUILD_RULE = 
endif

ifeq ($(BUILD_SPEC),default_romCompress)
AR               = arppc
AS               = ccppc
BOOT_EXTRA       = 
CC               = ccppc
CC_ARCH_SPEC     = -te500v2
CFLAGS           = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -Wall -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS        = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS_PROJECT = -g -te500v2 -ansi -fno-builtin -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CFLAGS_PROJECT   = -g -te500v2 -ansi -fno-builtin -Wall -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CONFIG_ALL       = ./all
CPP              = ccppc -E -P
CPU_VARIANT      = _ppc85XX_e500v2
EXTRA_MODULES    = $(SUB_TARGETS)
HEX_FLAGS        = 
LD               = ldppc
LD_LINK_PATH     = -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_standard -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2common_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2gnu -L$(VSB_DIR)/ppc/PPC32/e500v2common -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common
LD_PARTIAL       = ccppc -r -nostdlib -Wl,-X
LD_PARTIAL_FLAGS = -X -r
LIBS             = $(VX_OS_LIBS)
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_DEPEND    = -M -w
OPTION_GENERATE_DEPENDENCY_FILE = -MD
OPTION_INCLUDE_DIR = -I
OPTION_LANG_C    = -xc
OPTION_UNDEFINE_MACRO = -U
ROM_LINK_ADRS    = fff00000
ROM_SIZE         = 00100000 # number of bytes of ROM space
ROM_TEXT_ADRS    = fff00100 # ROM entry address
ROM_WARM_ADRS    = fff00108 # ROM warm entry address
SIZE             = sizeppc
TOOL_FAMILY      = gnu
USR_ENTRY        = usrInit
VMA_START        = 0x0
POST_BUILD_RULE  = 
POST_HEX_BUILD_RULE = 
endif

ifeq ($(BUILD_SPEC),default_romResident)
AR               = arppc
AS               = ccppc
BOOT_EXTRA       = 
CC               = ccppc
CC_ARCH_SPEC     = -te500v2
CFLAGS           = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -Wall -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS        = -DCPU_VARIANT=_ppc85XX_e500v2 -g -te500v2 -ansi -fno-builtin -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" -I$(PRJ_DIR) -I$(BSP_DIR) -I$(TGT_DIR)/h -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/drv -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -DPRJ_BUILD -D_WRS_KERNEL -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -I.
CFLAGS_AS_PROJECT = -g -te500v2 -ansi -fno-builtin -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -UCPU -DCPU=PPC85XX -UCPU_VARIANT -DCPU_VARIANT=_e500v2 -xassembler-with-cpp -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CFLAGS_PROJECT   = -g -te500v2 -ansi -fno-builtin -Wall -I./all -I$(TGT_DIR)/h -I$(WIND_HOME)/components/windml-5.3/h -I$(WIND_HOME)/components/webservices-1.7/h -I$(WIND_HOME)/components/webcli-4.8/target/h -I$(WIND_HOME)/components/opc-3.2/h -I$(WIND_HOME)/components/dcom-2.3/h -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/src -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/src -I$(WIND_HOME)/components/windml-5.3/osconfig/vxworks/h -I$(WIND_HOME)/components/ip_net2-6.8/osconfig/vxworks/h -I$(TGT_DIR)/src/hwif -I$(TGT_DIR)/src/hwif/h @./ipcom_ipdomain -I. -I$(TGT_DIR)/h/wrn/coreip -I$(TGT_DIR)/src/config -I$(TGT_DIR)/config/comps/src -I$(TGT_DIR)/config/comps/src/dosfs2 -I$(TGT_DIR)/src/drv -I$(TGT_DIR)/src/hwif/h -DCPU=PPC32 -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU_VARIANT=_ppc85XX_e500v2 -DIP_PORT_VXWORKS=68 -DINET -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"
CONFIG_ALL       = ./all
CPP              = ccppc -E -P
CPU_VARIANT      = _ppc85XX_e500v2
EXTRA_MODULES    = $(SUB_TARGETS)
HEX_FLAGS        = 
LD               = ldppc
LD_LINK_PATH     = -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_standard -L$(VSB_DIR)/ppc/PPC32/e500v2gnu_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2common_ppc85XX_e500v2 -L$(VSB_DIR)/ppc/PPC32/e500v2gnu -L$(VSB_DIR)/ppc/PPC32/e500v2common -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WS_ROOT_DIR)/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2gnu -L$(WIND_HOME)/components/obj/vxworks-6.8/krnl/lib/ppc/PPC32/e500v2common
LD_PARTIAL       = ccppc -r -nostdlib -Wl,-X
LD_PARTIAL_FLAGS = -X -r
LIBS             = $(VX_OS_LIBS)
NM               = nmppc
OPTION_DEFINE_MACRO = -D
OPTION_DEPEND    = -M -w
OPTION_GENERATE_DEPENDENCY_FILE = -MD
OPTION_INCLUDE_DIR = -I
OPTION_LANG_C    = -xc
OPTION_UNDEFINE_MACRO = -U
ROM_LINK_ADRS    = fff00000
ROM_SIZE         = 00100000 # number of bytes of ROM space
ROM_TEXT_ADRS    = fff00100 # ROM entry address
ROM_WARM_ADRS    = fff00108 # ROM warm entry address
SIZE             = sizeppc
TOOL_FAMILY      = gnu
USR_ENTRY        = usrInit
VMA_START        = 0x0
POST_BUILD_RULE  = 
POST_HEX_BUILD_RULE = 
endif

# override make definitions only below this line


# override make definitions only above this line

include $(TGT_DIR)/h/make/rules.project


## build-configuration info

sysALib.o: $(PRJ_DIR)/prjParams.h

ifeq ($(BUILD_SPEC),default)

usrAppInit.o:  $(PRJ_DIR)/usrAppInit.c 
	$(RM) usrAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrAppInit.c

usrRtpAppInit.o:  $(PRJ_DIR)/usrRtpAppInit.c 
	$(RM) usrRtpAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrRtpAppInit.c

test_file.o:  $(PRJ_DIR)/test/test_file.c 
	$(RM) test_file.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_file.c

test_uart.o:  $(PRJ_DIR)/test/test_uart.c 
	$(RM) test_uart.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_uart.c
endif

ifeq ($(BUILD_SPEC),default_rom)

usrAppInit.o:  $(PRJ_DIR)/usrAppInit.c 
	$(RM) usrAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrAppInit.c

usrRtpAppInit.o:  $(PRJ_DIR)/usrRtpAppInit.c 
	$(RM) usrRtpAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrRtpAppInit.c

test_file.o:  $(PRJ_DIR)/test/test_file.c 
	$(RM) test_file.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_file.c

test_uart.o:  $(PRJ_DIR)/test/test_uart.c 
	$(RM) test_uart.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_uart.c
endif

ifeq ($(BUILD_SPEC),default_romCompress)

usrAppInit.o:  $(PRJ_DIR)/usrAppInit.c 
	$(RM) usrAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrAppInit.c

usrRtpAppInit.o:  $(PRJ_DIR)/usrRtpAppInit.c 
	$(RM) usrRtpAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrRtpAppInit.c

test_file.o:  $(PRJ_DIR)/test/test_file.c 
	$(RM) test_file.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_file.c

test_uart.o:  $(PRJ_DIR)/test/test_uart.c 
	$(RM) test_uart.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_uart.c
endif

ifeq ($(BUILD_SPEC),default_romResident)

usrAppInit.o:  $(PRJ_DIR)/usrAppInit.c 
	$(RM) usrAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrAppInit.c

usrRtpAppInit.o:  $(PRJ_DIR)/usrRtpAppInit.c 
	$(RM) usrRtpAppInit.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/usrRtpAppInit.c

test_file.o:  $(PRJ_DIR)/test/test_file.c 
	$(RM) test_file.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_file.c

test_uart.o:  $(PRJ_DIR)/test/test_uart.c 
	$(RM) test_uart.o
	$(CC) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) -c $(PRJ_DIR)/test/test_uart.c
endif


## user defined rules

romStart.o : $(TGT_DIR)/config/comps/src/romStart.c
	$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) $(PROJECT_BSP_FLAGS_EXTRA) $(ROM_FLAGS_EXTRA) $< -o $@

romInit.o : $(WIND_HOME)/sbc8548-t9/SB8548M-s9_boot/wrSbc8548/romInit.s
	$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS_AS) $(CFLAGS_AS_OPTIONS_ALL) $(PROJECT_BSP_FLAGS_EXTRA) $(ROM_FLAGS_EXTRA) $< -o $@

linkSyms.o : $(PRJ_DIR)/linkSyms.c
	$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) $(PROJECT_BSP_FLAGS_EXTRA) $(PRJ_DIR)/linkSyms.c -o $@

sysLib.o : $(WIND_HOME)/sbc8548-t9/SB8548M-s9_boot/wrSbc8548/sysLib.c
	$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) $(PROJECT_BSP_FLAGS_EXTRA) $(WIND_HOME)/sbc8548-t9/SB8548M-s9_boot/wrSbc8548/sysLib.c -o $@

sysALib.o : $(WIND_HOME)/sbc8548-t9/SB8548M-s9_boot/wrSbc8548/sysALib.s
	$(AS) $(OPTION_OBJECT_ONLY) $(CFLAGS_AS) $(CFLAGS_AS_OPTIONS_ALL) $(PROJECT_BSP_FLAGS_EXTRA) $(WIND_HOME)/sbc8548-t9/SB8548M-s9_boot/wrSbc8548/sysALib.s -o $@

prjConfig.o : $(PRJ_DIR)/prjConfig.c
	$(CC) $(OPTION_OBJECT_ONLY) $(CFLAGS) $(CFLAGS_OPTIONS_ALL) $(PROJECT_BSP_FLAGS_EXTRA) $(PRJ_DIR)/prjConfig.c -o $@


src_build :

src_depend :

$(PRJ_FULL_PATH)/compObjs.lst : $(PRJ_MAKEFILE)
	@ $(RM) $@
	@ touch $@

src_clean :
	@ $(RM) compObjs.lst
