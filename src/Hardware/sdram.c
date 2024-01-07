/**
   @copyright None
   @file      sdram.c
   @author    Comment Vivre
   @date      2023-12-29
   @brief     None
*/
#include <main.h>

/**
   @function     SDRAM_Initialization_Sequence
   @brief        None
   @param[in]    hsdram: [输入/出]
**       Command: [输入/出]
   @return       None
   @date         2023-12-30
*/
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef * hsdram, FMC_SDRAM_CommandTypeDef * Command)
{
  __IO uint32_t tmpmrd = 0;
  /* Configure a clock configuration enable command */
  Command->CommandMode          = FMC_SDRAM_CMD_CLK_ENABLE;
  Command->CommandTarget        = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber    = 1;
  Command->ModeRegisterDefinition   = 0;
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  HAL_Delay(1);
  /* Configure a PALL (precharge all) command */
  Command->CommandMode          = FMC_SDRAM_CMD_PALL;
  Command->CommandTarget        = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber    = 1;
  Command->ModeRegisterDefinition   = 0;
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  /* Configure a Auto-Refresh command */
  Command->CommandMode          = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command->CommandTarget        = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber      = 8;
  Command->ModeRegisterDefinition   = 0;
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  /* Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4   |
    SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
    SDRAM_MODEREG_CAS_LATENCY_2           |
    SDRAM_MODEREG_OPERATING_MODE_STANDARD |
    SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  Command->CommandMode          = FMC_SDRAM_CMD_LOAD_MODE;
  Command->CommandTarget        = FMC_SDRAM_CMD_TARGET_BANK1;
  Command->AutoRefreshNumber    = 1;
  Command->ModeRegisterDefinition   = tmpmrd;
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);
  HAL_SDRAM_ProgramRefreshRate(hsdram, 677);
}
