#include "data_traps_support.h"
#include <stm32f4_discovery.h>
//dummy hear to keep head from getting optimized out
static volatile uintptr_t __data_traps_dummy_node __attribute__ ((section (".data"))) = 0;
//__data_traps_head getting optimized out init doesn't go through list
static volatile uintptr_t * __data_traps_head __attribute__ ((section (".data"))) = &__data_traps_dummy_node;  //list get built by compiler
static volatile uintptr_t __data_traps_count __attribute__ ((section (".data")))=2 ;
volatile uintptr_t __data_traps_key __attribute__ ((section (".data"))) =0xbeadeeee ;
volatile uintptr_t __data_traps_secret_value __attribute__ ((section (".data"))) =0x55555555;
extern void fault_handler (void);
void __data_traps_init(){
    volatile uintptr_t *cur_trap;
    volatile uintptr_t *next_trap;
    volatile uintptr_t *last_trap;
    uint32_t i;
    //don't deference the last node
    
    debug_printf("Count %u\n",__data_traps_count);
    debug_printf("data_trap_head 0x%08x\n",__data_traps_head);
    cur_trap = &__data_traps_head;
    for (i=0;i<__data_traps_count;i++){
        next_trap=(uintptr_t *)*cur_trap;
        *cur_trap=(*cur_trap)^__data_traps_key;
        debug_printf("cur_addr: 0x%08x next_addr: 0x%08x xored_val: 0x% 08x\n",\
                        cur_trap, next_trap, *cur_trap);
        cur_trap=next_trap;
    }
    //next should be the last node
    
    debug_printf("last_addr:0x%08x\n",cur_trap);
    *cur_trap=__data_traps_secret_value;
}

void __data_traps_check(){
    uint32_t i;
    volatile uintptr_t *next;
    BSP_LED_On(LED5);
    next=&__data_traps_head;
    for(i=0;i<__data_traps_count;i++){
        debug_printf("next: 0x%08x, xored:0x%08x\n",next,(*next)^__data_traps_key);
        next=(uintptr_t *)(*next^__data_traps_key);
    }
    if (*next!=__data_traps_secret_value){
        data_fault_handler();
    }
    BSP_LED_Off(LED5);
}


void __data_traps_set_head(uintptr_t*start,uint32_t length){
    __data_traps_head=start;
    __data_traps_count=length;

}

void __attribute__((weak))data_fault_handler(){
    fault_handler();
}
