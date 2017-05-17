

MCPU ?=cortex-m4


%.bc:%.c
	../../llvm_bin/bin/clang  -c -target arm-none-eabi -mcpu=$(MCPU)  -flto -emit-llvm -flto -fshort-enums $< -o $@ 

all: rt_edivirt_handlers.bc syscalls.bc
	../../llvm_bin/bin/llvm-link $^ -o rt_edivirt.o	


clean:
	rm -f *.o
	rm -f *.bc
