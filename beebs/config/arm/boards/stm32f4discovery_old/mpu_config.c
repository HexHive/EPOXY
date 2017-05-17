#include "mpu_config.h"

extern uint32_t __unsafestack_guard_start;

void Setup_MPU(void){

  MPU_Region_InitTypeDef MPU_InitStruct;
  /* Disable MPU */
  HAL_MPU_Disable();
  /* Default SYSMAP Protects System Control Block (VTOR and MPU)
     RW for privilege execution only, unprivelege no execution
  */


  /*
    REGION 0 
    Second Lowest Priority
    Define Every where as RW for all privileges
    Part has two memories (64KB at 0x1000000 and 128KB at 0x2000000)
    Covers from 0x00000000 to 0x80000000 (RAM and peripherals)
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;  
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;  // R
  HAL_MPU_ConfigRegion(&MPU_InitStruct);


 

  /* REGION 7 Highest Priority 
    Configure FLASH region as Read only and Executable  
    Set to 0x00000000 as the flash has two addresses
    0x00000000 and 0x08000000
  */
  MPU_InitStruct.BaseAddress = 0x00000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER7;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);



  /*REGION 6 
    SYS Control Reg 
    Must be protected as enables aliasing memory to address 0x00000000
    This would allow remapping the interrupt table
  */
  MPU_InitStruct.BaseAddress = SYSCFG_BASE;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512B;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER6;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RW;  //Also enables setting of some interrupts 
  MPU_InitStruct.DisableExec =MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  

  /* REGION 5
    Configure SCB region as Read only and Executable  
    Set to 0x00000000 as the flash has two addresses
    0x00000000 and 0x08000000
  */
  MPU_InitStruct.BaseAddress = 0xE000E000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4KB;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER5;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RW_URO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* REGION 4
    Unsafestack guard
  */
  MPU_InitStruct.BaseAddress = &__unsafestack_guard_start;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32B;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RW_URO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /*Default memory map should cover System Control Block*/

  /* Enable MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  __set_CONTROL(0x01);  // Drop privileges continue using MSP

}