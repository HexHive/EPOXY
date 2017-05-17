
#include <stdint.h>

#define __ASM __asm

void SVC_Handler();
void NMI_Handler();
void HardFault_Handler();


__attribute__((annotate("intercept:SVC_Handler")))__attribute__((annotate("no-virt")))void __edivirt_SVC_Handler(void){
  __ASM volatile (
                    "b $__edivirt_virtcall_handler \t\n"
                 );
}

__attribute__((annotate("intercept:NMI_Handler")))__attribute__((annotate("no-virt")))void __edivirt_NMI_Handler(void){
  __ASM volatile (
         "b $__edivirt_virtcall_handler \t\n"
         );

}

__attribute__((annotate("intercept:HardFault_Handler")))__attribute__((annotate("no-virt")))void __edivirt_HardFault_Handler(void){

  __ASM volatile (
                 "b $__edivirt_virtcall_handler \t\n"
                 );
}

#define SVC_FF_INSTR 0xdfff
#define SVC_FE_INSTR 0xdffe

// This function has to return from the interrupt if it handles the case
// When called r0 contains the stack ptr when entered ISR, and ISR lr is on top of stack
// |Stack     |  (Stack from ISR Entry)
// |++++++++++|
// |  ISR LR  |
// |  R3      |
// |  R2      |
// |  R1      |
// |  R0      | <---R0

__attribute__((annotate("no-virt")))__attribute__((naked))void __edivirt_virtcall_handler(uint32_t * context){

  //"sub r1,r1, 2\n\t"  \\adjust for thumb

  // ldr r2 value of instruction
  // see if instruction instruction is dffe or dfff (SVC 254 or SVC 255)
  // if is equal then return
  // else elevate privileges and exit ISR
  __ASM volatile("ldr r1, [r0, #44]\n\t"
                 "ldrh r2, [r1, #-2]\n\t"
                 "mov r3, 0xfffe \n\t"
                 "and r2, r2, r3 \n\t"
                 "mov r3, 0xdffe \n\t"
                 "teq r2,r3 \n\t"
                 "it ne \n\t"
                 "bxne lr\n\t" );
  //                 "it ne \n\t" removed from statement above because don't think I need
  __ASM volatile ("mrs r2,control \n\t"
                  "and r2, r2,0xFFFFFFFE\n\t"
                  "msr control, r2":::"r2"); //elevate privileges and return
 __ASM volatile("pop {r0,r1,r2,r3,pc} \n\t");  //restore state and return from ISR
     //           "bx  lr ");

/*  uint32_t pc = (context[6]-2);
  uint16_t instr = *(uint16_t *)(pc);
  //if was caused by SVC other than our intercepted on just return;
  if (!(instr == SVC_FF_INSTR  || instr ==SVC_FE_INSTR)){
        return;
  }
*/

}
