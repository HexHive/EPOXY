RAND_SEED:=
TOOLS:=3.9
CPU_TARGET:=cortex-m4
M_ARCH:=armv7em
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
LD:=arm-none-eabi-ld.gold
#LD:=arm-none-eabi-ld
BINUTILS_SIZE:=arm-none-eabi-size 

CLANG_OPTIONS:=
LINK_OPTIONS:=
# LLVM  (If you change one you probably need to change the other)
ifeq ($(TOOLS),3.9)
#@echo "Using LLVM 3.9"
CLANG:=$(HOME)/programs/llvm3.9/bin/bin/clang
LLVMgold_PLUGIN:=$(HOME)/programs/llvm3.9/bin/lib/LLVMgold.so
CLANG_VERSION_FLAGS:= -fshort-enums -ffreestanding
LLVMgold_VERSION_FLAGS+= --plugin-opt=save-temps

else  #Default Multicompiler LLVM 3.4
CLANG:=$(HOME)/arm/multicompiler/bin/clang
LLVMgold_PLUGIN:=$(HOME)/arm/multicompiler/lib/LLVMgold.so

CLANG_VERSION_FLAGS:= 
LLVMgold_VERSION_FLAGS:=
endif

# BUILD OPTIONS =======================================================================

ifdef SAFESTACK
CLANG_OPTIONS+=-fsanitize=safe-stack
#CLANG_OPTIONS+=-fsafe-stack-usp-storage=single-thread 
#CLANG_OPTIONS+=-msan-track-origins
#CLANG_OPTIONS+=-safe-stack-unsafestack-grow-upwards
#LINK_OPTIONS+=--plugin-opt=-fsanitize=safe-stack
#LINK_OPTIONS+=--plugin-opt=--debug-only=safestack
OPTION_SUFFIX:=$(OPTION_SUFFIX)--safestack
#used to change build dir not all diversification effect compilation, those that do need new dir
BUILD_SUFFIX:=$(BUILD_SUFFIX)--safestack
#LLVM_DEBUG_ONLY:=$(LLVM_DEBUG_ONLY)safestack
endif


ifdef DISABLE_INLINE
LINK_OPTIONS+=--plugin-opt=-disable-inlining
OPTION_SUFFIX:=$(OPTION_SUFFIX)--noinline
endif


ifneq ($(FUNCTION_PADDING), 0)
LINK_OPTIONS+=--plugin-opt=-randomize-function-list=$(FUNCTION_PADDING)
#LINK_OPTIONS+=--plugin-opt=--debug-only=CodePadding
OPTION_SUFFIX:=$(OPTION_SUFFIX)--randomize-functions=$(FUNCTION_PADDING)
#LLVM_DEBUG_ONLY:=$(LLVM_DEBUG_ONLY)CodePadding
endif

ifneq ($(GLOBAL_PADDING), 0)
CLANG_OPTIONS+=-DINSERT_DATA_TRAPS
LINK_OPTIONS+=--plugin-opt=-global-merge-max-add-traps=0xFFFFFFFF
LINK_OPTIONS+=--plugin-opt=-global-merge-pad-data=$(GLOBAL_PADDING) 
#LINK_OPTIONS+=--plugin-opt=-global-merge-randomize 
LINK_OPTIONS+=--plugin-opt=-global-merge-group-by-use=false
#LINK_OPTIONS+=--plugin-opt=-global-merge-max-data-section-pad=$(MAX_DATA_SECTION_PADDING)
OPTION_SUFFIX:=$(OPTION_SUFFIX)--trap-globals=$(GLOBAL_PADDING)
#BUILD_SUFFIX:=$(BUILD_SUFFIX)--trap-globals=$(GLOBAL_PADDING)
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

#LINK_OPTIONS+=--plugin-opt=--help
# ======================== END PATHS To TOOLS=====================================
#LIBS
SYS_INC:=$(HOME)/programs/newlib/gcc/arm-none-eabi/include
SYS_LIBS:=-L $(HOME)/programs/newlib/gcc/arm-none-eabi/lib/armv7e-m \
	      -L $(HOME)/arm/



ifeq ($(CPU_TARGET),cortex-m3)
SYS_LIBS:=-L $(HOME)/arm/newlib/bin/arm-none-eabi/lib/armv7-m
LLVMgold_VERSION_FLAGS+= #--plugin-opt=-float-abi=soft
endif

# Solution for finding current dir name from
# http://stackoverflow.com/questions/18136918/how-to-get-current-directory-of-your-makefile
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

STARTUPDIR = $(DIR_PREFIX)stm32_lib/startup
ASOURCES+=$(shell find -L $(STARTUPDIR) -name '*.s')


# =============================================================================
#Use part of BEEBS build system to build this
top_srcdir :=$(shell readlink `pwd`../..)

ARCH=arm
BOARD=stm32f4discovery
CHIPSUPPORT_C = 1
CHIP = stm32f407

DIR_NAME=$(shell basename $(CURDIR))

include Makefile.am
include $(top_srcdir)/config/$(ARCH)/chips/$(CHIP)/chip.mk

DEFINES = -DHAVE_CHIPSUPPORT_H

LINKER_SCRIPT = $(top_srcdir)/config/$(ARCH)/chips/$(CHIP)/chip.ld

COBJECTS=$($(DIR_NAME)_SOURCES)

INC = $(COMMON_INC)


#arm-none-eabi-gcc $($(DIR)_SOURCES) $(AM_CFLAGS) $(DEFINES) $(CHIP_CFLAGS) $(CHIP_LDFLAGS) -o $(DIR).elf



# COMPLIER FLAGS
DIRECTIVES:=-DHAVE_CHIPSUPPORT_H -DSTM32F4DISCOVERY -DSTM32F407VGTx -DSTM32 -DSTM32F4 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx 
#ARCH_FLAGS:= -target arm-none-eabi -mcpu=$(CPU_TARGET) -mthumb -mfloat-abi=soft
ARCH_FLAGS:= -target arm-none-eabi -mcpu=$(CPU_TARGET)
#OPT_FLAGS:= -O0 -flto -fmessage-length=0 -ffunction-sections -g $(CLANG_VERSION_FLAGS) 
OPT_FLAGS:= -fshort-enums -fno-exceptions -O1 -flto -ffreestanding -nostdlib -fmessage-length=0 -ffunction-sections -g
#LIB_FLAGS:= -ffreestanding 
LIB_FLAGS:=
CLANG_FLAGS:= $(OPT_FLAGS) $(ARCH_FLAGS) $(LIB_FLAGS)

LD_SCRIPT:=$(DIR_PREFIX)SafeStackLinkerScript.ld

# LINKER FLAGS =======================================================================================

#PLUGIN_FLAGS:=-plugin=$(LLVMgold_PLUGIN) $(LLVMgold_VERSION_FLAGS) $(DIVERSIFICATION_FLAGS) --plugin-opt=mcpu=$(CPU_TARGET) 
#PLUGIN_FLAGS:=-plugin=$(LLVMgold_PLUGIN) $(LLVMgold_VERSION_FLAGS) $(DIVERSIFICATION_FLAGS) 
PLUGIN_FLAGS:=-plugin=$(LLVMgold_PLUGIN) $(LLVMgold_VERSION_FLAGS) 


GCC_LD_FLAGS:= -T$(LD_SCRIPT) -g $(LINK_OPTIONS)
LD_GROUP_FLAGS:=
LD_FLAGS:=$(PLUGIN_FLAGS) -nostdlib $(GCC_LD_FLAGS) $(SYS_LIBS) $(LD_GROUP_FLAGS) #GROUP FLAGS need to be last
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

data_trap_declarations:
	python $(DIR_PREFIX)tools/create_data_traps.py --padding $(GLOBAL_PADDING) >$(BUILD_DIR)/data_traps_delarations.h

$(BUILD_DIR)/tools/data_traps/%.o:$(DIR_PREFIX)tools/data_traps/%.c data_trap_declarations
	$(CLANG) $(CLANG_FLAGS) $(CLANG_OPTIONS) $(INCLUDES) -I$(BUILD_DIR) $(DIRECTIVES) -c $< -o $@
	@echo Compiling $^ to $@

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
#	@echo $(addprefix $(BUILD_DIR)/,$(patsubst %.c, %.o, $(shell find -L src/$(subst $(OPTION_SUFFIX), ,$(basename $(notdir $@))) -name '*.c'))) $(LD_GROUP_FLAGS)
	$(LD) $(LINKER_SYM_OVERRIDES) $(LD_FLAGS) --plugin-opt=-info-output-file=$(patsubst %.elf,%.stats,$@) $(OBJS) -o $(@) -lc



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

