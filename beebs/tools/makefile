RAND_SEED:=

CPU_TARGET:=cortex-m4
M_ARCH:=armv7-m
#Where to find other souce files
DIR_PREFIX:=../../
BUILD_ROOT:=build
BIN_DIR:=$(DIR_PREFIX)bin
MAKE_FILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(notdir $(patsubst %/,%,$(dir $(MAKE_FILE_PATH))))

GLOBAL_PADDING:= 0
FUNCTION_PADDING:=0

# ======================== PATHS To TOOLS=====================================

# FROM BINUTILS
LD:=../../../../gcc/bins/bin/arm-none-eabi-ld.bfd
#LD:=arm-none-eabi-ld
BINUTILS_SIZE:=../../../../gcc/bins/bin/arm-none-eabi-size

CLANG_OPTIONS:=
LINK_OPTIONS:=

#To use these options define Environmental Variable TOOLS
#otherwise they are overwritten

CLANG:=clang
LLVMgold_PLUGIN:=$(LLVMGold)
CLANG_VERSION_FLAGS:= -fshort-enums -ffreestanding
LLVMgold_VERSION_FLAGS+= --plugin-opt=save-temps
PRIVILEGE_TEMPLATE:=$(PRIVILEGE_TEMPLATE)

ifndef TOOLS
TOOLS_ROOT:=../../../../llvm_bin
CLANG:=$(TOOLS_ROOT)/bin/clang
LLVMgold_PLUGIN:=$(TOOLS_ROOT)/lib/LLVMgold.so
CLANG_VERSION_FLAGS:= -fshort-enums -ffreestanding
LLVMgold_VERSION_FLAGS+= --plugin-opt=save-temps

PRIVILEGE_TEMPLATE:=../../../EPOXY-rt/permissions_table.txt
RUNTIME_SUPPORT:=../../../EPOXY-rt/rt_edivirt.o
SYS_INC:=../../../../gcc/bins/arm-none-eabi/include/
SYS_LIBS:=-L ../../../../gcc/bins/arm-none-eabi/lib/thumb/v7e-m/ \
		-L ../../../../gcc/bins/lib/gcc/arm-none-eabi/6.3.1/thumb/v7e-m/


endif


# BUILD OPTIONS =======================================================================

ifdef SAFESTACK
CLANG_OPTIONS+=-fsanitize=safe-stack
OPTION_SUFFIX:=$(OPTION_SUFFIX)--safestack
BUILD_SUFFIX:=$(BUILD_SUFFIX)--safestack
endif


ifdef DISABLE_INLINE
LINK_OPTIONS+=--plugin-opt=-disable-inlining
OPTION_SUFFIX:=$(OPTION_SUFFIX)--noinline
endif


ifneq ($(FUNCTION_PADDING), 0)
LINK_OPTIONS+=--plugin-opt=-randomize-function-list=$(FUNCTION_PADDING)
OPTION_SUFFIX:=$(OPTION_SUFFIX)--randomize-functions=$(FUNCTION_PADDING)
endif

ifneq ($(GLOBAL_PADDING), 0)
LINK_OPTIONS+=--plugin-opt=-global-merge-pad-data=$(GLOBAL_PADDING)
OPTION_SUFFIX:=$(OPTION_SUFFIX)--trap-globals=$(GLOBAL_PADDING)
BUILD_SUFFIX:=$(BUILD_SUFFIX)--trap-globals
LLVM_DEBUG_ONLY:=$(LLVM_DEBUG_ONLY)global-merge
endif

ifdef RAND_SEED
LINK_OPTIONS+=--plugin-opt=-rng-seed=$(RAND_SEED)
OPTION_SUFFIX:=$(OPTION_SUFFIX)--seed=$(RAND_SEED)
else
#LINK_OPTIONS+=--plugin-opt=-rng-seed=12345678
#OPTION_SUFFIX:=$(OPTION_SUFFIX)--seed=12345678
endif

ifdef LLVM_ONLY
LINK_OPTIONS+=--plugin-opt=-emit-llvm
endif

ifdef UNSAFE_STACK_COOKIE_VALUE
CLANG_OPTIONS+=-DUNSAFE_STACK_COOKIE_VALUE=$(UNSAFE_STACK_COOKIE_VALUE)
endif

ifdef LLVM_DEBUG_ONLY
LINK_OPTIONS+=--plugin-opt=--debug-only=$(LLVM_DEBUG_ONLY)
endif

ifdef HELP
LINK_OPTIONS+=--plugin-opt=--help
endif

ifdef RAND_REGS
LINK_OPTIONS+=--plugin-opt=--randomize-regalloc
BUILD_SUFFIX:=$(BUILD_SUFFIX)--randomize-regalloc--seed=$(RAND_SEED)
OPTION_SUFFIX:=$(OPTION_SUFFIX)--rand_regs
endif

ifdef REDUCE_PRIV
LINK_OPTIONS+=--plugin-opt=-add-edivirt
LINK_OPTIONS+=--plugin-opt=-reduce-privileges=$(PRIVILEGE_TEMPLATE)
CLANG_OPTIONS+=-DADD_VIRT
BUILD_SUFFIX:=$(BUILD_SUFFIX)--reduce_priv
OPTION_SUFFIX:=$(OPTION_SUFFIX)--reduce_priv
endif

ifndef NO_STATS
LINK_OPTIONS+=--plugin-opt=-stats
endif

ifdef STACK_LOC
LINKER_SYM_OVERRIDES+=--defsym=_estack=$(STACK_LOC)
endif

ifdef DATA_LOC
LINKER_SYM_OVERRIDES+=--defsym=_start_data=$(DATA_LOC)
endif

ifdef UNSAFE_STACK_LOC
LINKER_SYM_OVERRIDES+=--defsym=_eunsafe_stack=$(UNSAFE_STACK_LOC)
endif

# Solution for finding current dir name from
# http://stackoverflow.com/questions/18136918/how-to-get-current-directory-of-your-makefile
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))


HALDIR= $(DIR_PREFIX)stm32_lib/HAL_Driver
BOARDDIR= $(DIR_PREFIX)config/arm/boards/stm32f4discovery/

CMSISDIR= $(DIR_PREFIX)stm32_lib/CMSIS
CSOURCES+=$(shell find -L $(BOARDDIR) -name '*.c')
CSOURCES += $(DIR_PREFIX)/stm32_lib/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery.c
CSOURCES += $(HALDIR)/Src/stm32f4xx_hal.c \
	$(HALDIR)/Src/stm32f4xx_hal_cortex.c \
	$(HALDIR)/Src/stm32f4xx_hal_gpio.c \
	$(HALDIR)/Src/stm32f4xx_hal_dma.c \
  	$(HALDIR)/Src/stm32f4xx_hal_spi.c \
  	$(HALDIR)/Src/stm32f4xx_hal_i2c.c \
  	$(HALDIR)/Src/stm32f4xx_hal_rcc.c \
	$(HALDIR)/Src/stm32f4xx_hal_rcc_ex.c


INC+=$(DIR_PREFIX)/stm32_lib/Drivers/BSP/STM32F4-Discovery
INC+=$(CMSISDIR)/device
INC+=$(CMSISDIR)/core
INC+=$(HALDIR)/Inc
INC+=$(CMSISDIR)/Device/ST/STM32F4xx/Include
INC+=$(CMSISDIR)/Include
INC+=$(shell find -L $(DIR_PREFIX)include -name '*.h' -exec dirname {} \;| uniq)
INC+=$(shell find -L $(BOARDDIR) -name '*.h' -exec dirname {} \; | uniq)
# =============================================================================
#Use part of BEEBS build system to build this
#top_srcdir :=$(shell readlink -f `pwd`/../..)
top_srcdir:=../..

ARCH=arm
BOARD=stm32f4discovery
CHIPSUPPORT_C = 1
CHIP = stm32f407

DIR_NAME=$(shell basename $(CURDIR))
#Bring in test bench source
include Makefile.am
#Bring in startup code
include $(top_srcdir)/config/$(ARCH)/chips/$(CHIP)/chip.mk

LD_SCRIPT = $(top_srcdir)/config/$(ARCH)/chips/$(CHIP)/chip.ld

CSOURCES+=$($(DIR_NAME)_SOURCES)

INC+=$($(DIR_NAME)_HEADERS)
INC +=$(COMMON_INC)

#INCLUDES=-Iinclude $(addprefix -I , $(INC)) -I$(SYS_INC)
INCLUDES=-Iinclude $(addprefix -I , $(INC)) -I$(SYS_INC)
BENCHMARK_DIRECTIVES=$($(DIR_NAME)_CFLAGS)

# COMPLIER FLAGS
DIRECTIVES:=$($(DIR_NAME)_CFLAGS) -DSTM32F4DISCOVERY -DSTM32F407VGTx -DSTM32 -DSTM32F4 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx
ARCH_FLAGS:= -target arm-none-eabi -mcpu=$(CPU_TARGET) -mfloat-abi=soft
OPT_FLAGS:= -fshort-enums -fno-exceptions -O0 -flto -ffreestanding -fmessage-length=0 -ffunction-sections -g
LIB_FLAGS:=
CLANG_FLAGS:= $(OPT_FLAGS) $(ARCH_FLAGS) $(LIB_FLAGS)

# LINKER FLAGS =======================================================================================
PLUGIN_FLAGS:=-plugin=$(LLVMgold_PLUGIN) $(LLVMgold_VERSION_FLAGS)
GCC_LD_FLAGS:= -T$(LD_SCRIPT) -g $(LINK_OPTIONS) --plugin-opt=-mcpu=$(CPU_TARGET) --plugin-opt=-float-abi=soft
LD_GROUP_FLAGS:=
LD_FLAGS:=$(PLUGIN_FLAGS) $(GCC_LD_FLAGS) $(SYS_LIBS) $(LD_GROUP_FLAGS) #GROUP FLAGS need to be last

CLANG_LD_FLAGS:= -nostdlib $(GCC_LD_FLAGS) $(SYS_LIBS) $(LD_GROUP_FLAGS) $(ARCH_FLAGS)
#COMPILE TARGETS =======================================================================================

ifdef BUILD_SUFFIX
	BUILD_DIR:=$(BUILD_ROOT)/$(BUILD_SUFFIX)
else
	BUILD_DIR:=$(BUILD_ROOT)/default
endif
COBJECTS=$(CSOURCES:%.c=%.o)
SOBJECTS=$(ASOURCES:%.s=%.o)
OBJS=$(addprefix $(BUILD_DIR)/,$(subst ../,,$(COBJECTS)))
OBJS+=$(addprefix $(BUILD_DIR)/,$(subst ../,,$(SOBJECTS)))

#Creates a directoy in the build directory for every directory
#that has a .c or s file (A bit overkill but will work)
#DIRS=
BUILD_DIRS:=$(sort $(dir $(OBJS)))
BUILD_DIRS+=$(BIN_DIR)

ELF=$(addsuffix .elf,$(addprefix $(BIN_DIR)/,$(addsuffix $(OPTION_SUFFIX),$(CURRENT_DIR))))

.PHONY: clean all checkdirs debug data_trap_declarations

all: checkdirs $(ELF)
#	@ls -l $(BIN_DIR)
	@$(BINUTILS_SIZE) $(ELF)
	@echo All Done
	@echo sleep 1
	@echo Build $(shell ls -l $(BIN_DIR)/*.elf|wc -l) Binaries

checkdirs:$(BUILD_DIRS)

$(BUILD_DIR)/%.o:$(DIR_PREFIX)%.s
	@$(CLANG) $(CLANG_FLAGS) $(INCLUDES) $(DIRECTIVES) -c $< -o $@
	@echo Assembling $^ to $@

$(BUILD_DIR)/%.o:$(DIR_PREFIX)%.c
	$(CLANG) $(CLANG_FLAGS) $(CLANG_OPTIONS) $(INCLUDES) $(DIRECTIVES) -c $< -o $@
	@echo Compiling $^ to $@

$(BUILD_DIR)/%.o:../%.s
	@$(CLANG) $(CLANG_FLAGS) $(INCLUDES) $(DIRECTIVES) -c $< -o $@
	@echo Assembling $^ to $@

$(BUILD_DIR)/%.o:../%.c
	$(CLANG) $(CLANG_FLAGS) $(CLANG_OPTIONS) $(INCLUDES) $(DIRECTIVES) -c $< -o $@
	@echo Compiling $^ to $@

$(BUILD_DIR)/%.o:%.s
	@$(CLANG) $(CLANG_FLAGS) $(INCLUDES) $(DIRECTIVES) -c $< -o $@
	@echo Assembling $^ to $@

$(BUILD_DIR)/%.o:%.c
	$(CLANG) $(CLANG_FLAGS) $(CLANG_OPTIONS) $(INCLUDES) $(DIRECTIVES) -c $^ -o $@
	@echo Compiling $< to $@

$(ELF): $(OBJS)
	@echo Linking $@
	$(LD) $(LINKER_SYM_OVERRIDES) $(LD_FLAGS) $(RUNTIME_SUPPORT) --plugin-opt=O2 --plugin-opt=-info-output-file=$(patsubst %.elf,%.stats,$@) $(OBJS)  -o $(@) --start-group -lc -lm -lgcc --end-group

debug:
	@echo $(BUILD_DIRS)

echo_objs:
	@echo $(addsuffix $(\n),$(OBJS))
#	@echo $(current_dir)

clean:
	rm -rf $(BUILD_ROOT)
	rm -f $(BIN_DIR)/$(CURRENT_DIR)*

$(BUILD_DIRS):
	@mkdir -p $@
