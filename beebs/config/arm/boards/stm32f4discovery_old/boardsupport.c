/* Copyright (C) 2014 Embecosm Limited and University of Bristol

   Contributor James Pallister <james.pallister@bristol.ac.uk>

   This file is part of the Bristol/Embecosm Embedded Benchmark Suite.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include <stm32f4xx_hal.h>
#include <support.h>
#include <stm32f4_discovery.h>
#ifdef REDUCE_PRIV
  #include "mpu_config.h"
#endif

#define SAFESTACK_FAULT 20
#define BUS_FAULT 30
#define HARD_FAULT 40
#define USAGE_FAULT 50
#define MEM_FAULT 60 
#define FUNCTION_TRAP_FAULT 70
#define DATA_FAULT 80


extern uint32_t __safestack_unsafe_guard;  //should be defined in linker script
extern uint32_t __safestack_unsafe_stack_cookie;

static void SystemClock_Config(void);
void __safestack_unsafe_stack_error_handler();
extern uint32_t __unsafestack_guard_start;



/*void Default_Handler(void){
    BSP_LED_On(LED6);
    while(1);
}*/

void end_reached () __attribute__ ((used));
void end_reached (){
    volatile int i =0;
    for(i=0; i<0xFFFF;i++)
        ;
    HAL_NVIC_SystemReset();
}

void initialise_board()
{
    #ifdef REDUCE_PRIV
      Setup_MPU();
    #endif
    //NVIC_DisableIRQ(SysTick_IRQn); Illegal use of this api
    NVIC_DisableIRQ(WWDG_IRQn);
    if(HAL_Init()!= HAL_OK)
    {
        BSP_LED_On(LED5);
        while(1);
    }
    SystemClock_Config();
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);
    BSP_LED_Init(LED5);
    BSP_LED_Init(LED6);
    BSP_LED_Off(LED3);
    BSP_LED_Off(LED4);
    BSP_LED_Off(LED5);
    BSP_LED_Off(LED6);
    //NVIC_DisableIRQ(SysTick_IRQn);
    //NVIC_DisableIRQ(WWDG_IRQn);
    //NVIC_EnableIRQ(BusFault_IRQn);
    //NVIC_EnableIRQ(UsageFault_IRQn);
    //NVIC_EnableIRQ(MemManage_IRQn);
    //NVIC_EnableIRQ(-13); //HardFault
    //__disable_irq();
    //__disable_fault_irq();
}

void start_trigger()
{
    BSP_LED_On(LED3);
}

void stop_trigger()
{
    BSP_LED_Off(LED3);
    BSP_LED_On(LED4);
    end_reached();
}

void exit (int status)
{
  //likely caused by heap being to small
  BSP_LED_On(LED5);
  BSP_LED_On(LED6);
  while (1) {}    // Make sure we hang here 
}

void __attribute__((noinline,optnone))fault_handler(uint32_t code){
    BSP_LED_On(LED5);
    BSP_LED_On(LED6);
    BSP_LED_On(LED3);
    BSP_LED_On(LED4);
    while(1)
      ;
}

void __assert_fail (const char *assertion,
             const char *file, unsigned int line, const char *function){
  uint32_t i;
  while(1){
    for (i=0;i<0xFFFF;i++){
      ;
    }
    BSP_LED_Toggle(LED5);
    BSP_LED_Toggle(LED6);
  }
    
  
}

void __attribute__((used))__safestack_check_unsafe_stack_cookie(){
    BSP_LED_On(LED6);
    if((uint32_t)__safestack_unsafe_stack_cookie != (uint32_t)__safestack_unsafe_guard){
        __safestack_unsafe_stack_error_handler();
    }
    BSP_LED_Off(LED6);
}

void __safestack_unsafe_stack_error_handler(){
    fault_handler(SAFESTACK_FAULT);
}

void BusFault_Handler(){
    fault_handler(BUS_FAULT);
}

void HardFault_Handler(uint32_t code){
  #ifdef REDUCE_PRIV
  uint32_t instruction;
  uint32_t new_ret_addr; 
  __ASM volatile("ldr %0, [sp, #24] \n\t"
                 "ldr %1, [%0, #4] \n\t"
                 :"=r"(new_ret_addr),"=r"(instruction): : "memory"
                 );
  if((instruction &0xFFFF) ==0xdfff && code == 0){
    __ASM volatile ("str %0, [sp, #24] \n\t"
                   : :"r"(new_ret_addr): "memory");
    switch (code){
      case 0:  //__enable_irq()
          __ASM volatile ("cpsie i" : : :"memory"); 
        return;
      case 6:
          __ASM volatile ("cpsie f" : : :"memory");
          return;
      default:
        ;
    }
  }
  #endif

  fault_handler(HARD_FAULT);
}

void UsageFault_Handler(){
    fault_handler(USAGE_FAULT);//need to keep from getting optimized out
}

void MemManage(){
    fault_handler(MEM_FAULT);
}
void SysTick_Handler(){
}

void __attribute__((used))__default_invalid_addr_handler(){
  fault_handler(FUNCTION_TRAP_FAULT);
}

void data_fault_handler(){
    fault_handler(DATA_FAULT);
}


void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  //HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  //HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  //HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

__attribute__((optnone)) void SVC_Handler(uint32_t code, void * p,void * r1){
  /*
  * Elivates privileges for specific CMSIS function 
  * calls
  * assumes it is the use for making a SVC call
  */
  uint32_t *return_loc;
  __ASM volatile("mov %0, sp" : "=r"(return_loc) : :);
  //return_loc;  //Account for this value
 #ifdef REDUCE_PRIV
  switch (code){
    case 0:
      __ASM volatile ("cpsie i" : : : "memory");
      break;
    case 1:
      __ASM volatile ("cpsid i" : : : "memory");
      break;
    case 2:
      __ASM volatile ("MSR psp, %0" : : "r" (p) : "sp");
      break;
    case 3:
      /*  Need to build the ISR context on the new stack
      then do switch *
      __ASM volatile ("sub sp 
                      LDM sp, {r0,r2,r3}\n\t"
                      "STM r1!, [r1,r2,r3]!\n\t"
                      "str r0, [r1]\n\t")
      __ASM volatile ("MSR msp, %0" : : "r1" (p): "r0","r1","r2","r3","sp");
      */
      break;
    case 4:
      __ASM volatile ("MSR primask, %0" : : "r" (p) : "memory");
      break;
    case 5:
      __ASM volatile ("cpsie f" : : : "memory");
      break;
    case 6:
      __ASM volatile ("cpsid f" : : : "memory");
      break;
    case 7:
      __ASM volatile ("MSR basepri, %0" : : "r" (p) : "memory");
      break;
    case 8:
      __ASM volatile ("MSR basepri_max, %0" : : "r" (p) : "memory");
      break;
    case 9:
      __ASM volatile ("MSR faultmask, %0" : : "r" (p) : "memory");
      break;
    default:
      (*return_loc)=Run_CMSIS_Elevated(code,p,r1);
  }
  #endif
}