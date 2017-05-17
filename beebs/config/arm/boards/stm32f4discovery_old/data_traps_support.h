#ifndef __DATA_TRAPS_SUPPORT_H__
#define __DATA_TRAPS_SUPPORT_H__
#include <stdint.h>

#define DATA_TRAPS_DEBUG 0

#if DATA_TRAPS_DEBUG
    #include <stdio.h>
    #define debug_printf(fmt,...)\
        do{if(DATA_TRAPS_DEBUG){printf(fmt,##__VA_ARGS__);}}while(0)
#else
    #define uintptr uint32_t
    #define debug_printf(fmt,...)\
        ;
#endif

void __data_traps_init();

void __attribute__((used,noinline,optnone))__data_traps_check();

void __attribute__((weak))data_fault_handler();


void __data_traps_set_head(uintptr_t * start,uint32_t length);


#endif
