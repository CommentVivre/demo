#include "qspi_w25q64.h"

//QSPI进入内存映射模式（执行QSPI代码必备前提，为了减少引入的文件，
//除了GPIO驱动外，其他的外设驱动均采用寄存器形式）
void QSPI_Enable_Memmapmode(void)
{
  uint32_t tempreg = 0;
  __IO uint32_t * data_reg = &QUADSPI->DR;
  GPIO_InitTypeDef qspi_gpio;
  RCC->AHB4ENR |= 1 << 1;           //使能PORTB时钟
  RCC->AHB4ENR |= 1 << 5;           //使能PORTF时钟
  RCC->AHB3ENR |= 1 << 14;          //QSPI时钟使能
  qspi_gpio.Pin = GPIO_PIN_6;       //PB6 AF10
  qspi_gpio.Mode = GPIO_MODE_AF_PP;
  qspi_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  qspi_gpio.Pull = GPIO_NOPULL;
  qspi_gpio.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOB, &qspi_gpio);
  qspi_gpio.Pin = GPIO_PIN_2;       //PB2 AF9
  qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOB, &qspi_gpio);
  qspi_gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7; //PF6,7 AF9
  qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOF, &qspi_gpio);
  qspi_gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9; //PF8,9 AF10
  qspi_gpio.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOF, &qspi_gpio);
  //QSPI设置，参考QSPI实验的QSPI_Init函数
  RCC->AHB3RSTR |= 1 << 14; //复位QSPI
  RCC->AHB3RSTR &= ~(1 << 14); //停止复位QSPI
  
  while (QUADSPI->SR & (1 << 5)); //等待BUSY位清零
  
  QUADSPI->CR = 0X01000310;   //设置CR寄存器,这些值怎么来的，请参考QSPI实验/看H750参考手册寄存器描述分析
  QUADSPI->DCR = 0X00160401;  //设置DCR寄存器
  QUADSPI->CR |= 1 << 0;    //使能QSPI
  
  // 注意:QSPI QE位的使能，在QSPI烧写算法里面，就已经设置了
  // 所以,这里可以不用设置QE位，否则需要加入对QE位置1的代码
  // 不过,代码必须通过仿真器下载,直接烧录到外部QSPI FLASH,是不可用的
  // 如果想直接烧录到外部QSPI FLASH也可以用,则需要在这里添加QE位置1的代码
  
  //W25QXX进入QPI模式（0X38指令）
  while (QUADSPI->SR & (1 << 5)); //等待BUSY位清零
  
  QUADSPI->CCR = 0X00000138;  //发送0X38指令，W25QXX进入QPI模式
  
  while ((QUADSPI->SR & (1 << 1)) == 0); //等待指令发送完成
  
  QUADSPI->FCR |= 1 << 1;   //清除发送完成标志位
  
  //W25QXX写使能（0X06指令）
  while (QUADSPI->SR & (1 << 5)); //等待BUSY位清零
  
  QUADSPI->CCR = 0X00000106;  //发送0X06指令，W25QXX写使能
  
  while ((QUADSPI->SR & (1 << 1)) == 0); //等待指令发送完成
  
  QUADSPI->FCR |= 1 << 1;   //清除发送完成标志位
  
  //W25QXX设置QPI相关读参数（0XC0）
  while (QUADSPI->SR & (1 << 5)); //等待BUSY位清零
  
  QUADSPI->CCR = 0X030003C0;  //发送0XC0指令，W25QXX读参数设置
  QUADSPI->DLR = 0;
  
  while ((QUADSPI->SR & (1 << 2)) == 0); //等待FTF
  
  *(__IO uint8_t *)data_reg = 3 << 4;  //设置P4&P5=11,8个dummy clocks,104M
  QUADSPI->CR |= 1 << 2;    //终止传输
  
  while ((QUADSPI->SR & (1 << 1)) == 0); //等待数据发送完成
  
  QUADSPI->FCR |= 1 << 1;   //清除发送完成标志位
  
  while (QUADSPI->SR & (1 << 5)); //等待BUSY位清零
  
  //MemroyMap 模式设置
  while (QUADSPI->SR & (1 << 5)); //等待BUSY位清零
  
  QUADSPI->ABR = 0;       //交替字节设置为0，实际上就是W25Q 0XEB指令的,M0~M7=0
  tempreg = 0XEB;       //INSTRUCTION[7:0]=0XEB,发送0XEB指令（Fast Read QUAD I/O）
  tempreg |= 3 << 8;      //IMODE[1:0]=3,四线传输指令
  tempreg |= 3 << 10;     //ADDRESS[1:0]=3,四线传输地址
  tempreg |= 2 << 12;     //ADSIZE[1:0]=2,24位地址长度
  tempreg |= 3 << 14;     //ABMODE[1:0]=3,四线传输交替字节
  tempreg |= 0 << 16;     //ABSIZE[1:0]=0,8位交替字节(M0~M7)
  tempreg |= 6 << 18;     //DCYC[4:0]=6,6个dummy周期
  tempreg |= 3 << 24;     //DMODE[1:0]=3,四线传输数据
  tempreg |= 3 << 26;     //FMODE[1:0]=3,内存映射模式
  QUADSPI->CCR = tempreg;   //设置CCR寄存器
  //设置QSPI FLASH空间的MPU保护
  SCB->SHCSR &= ~(1 << 16); //禁止MemManage
  MPU->CTRL &= ~(1 << 0);   //禁止MPU
  MPU->RNR = 0;         //设置保护区域编号为0(1~7可以给其他内存用)
  MPU->RBAR = 0X90000000;   //基地址为0X9000 000,即QSPI的起始地址
  MPU->RASR = 0X0303002D;   //设置相关保护参数(禁止共用,允许cache,允许缓冲),详见MPU实验的解析
  MPU->CTRL = (1 << 2) | (1 << 0); //使能PRIVDEFENA,使能MPU
  SCB->SHCSR |= 1 << 16;    //使能MemManage
}

QSPI_HandleTypeDef hqspi;

void HAL_QSPI_MspInit(QSPI_HandleTypeDef * qspiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  
  if (qspiHandle->Instance == QUADSPI)
  {
    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.PLL2.PLL2M = 5;
    PeriphClkInitStruct.PLL2.PLL2N = 192;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 4;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
    
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      return;
    }
    
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PB6     ------> QUADSPI_BK1_NCS
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}


void MX_QUADSPI_Init(void)
{
  hqspi.Instance                = QUADSPI;                  // QSPI外设
  /*本例程选择 pll2_r_ck 作为QSPI的内核时钟，速度为250M，再经过2分频得到125M驱动时钟，
    关于 QSPI内核时钟 的设置，请参考 main.c文件里的 SystemClock_Config 函数*/
  // 需要注意的是，当使用内存映射模式时，这里的分频系数不能设置为0！！否则会读取错误
  hqspi.Init.ClockPrescaler     = 2;                      // 时钟分频值，将QSPI内核时钟进行 1+1 分频得到QSPI通信驱动时钟
  hqspi.Init.FifoThreshold      = 4;                      // FIFO阈值
  hqspi.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE; // 半个CLK周期之后进行采样
  hqspi.Init.FlashSize          = POSITION_VAL(0X800000) - 1;                     // flash大小，FLASH 中的字节数 = 2^[FSIZE+1]，核心板采用是8M字节的W25Q64，这里设置为22
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_5_CYCLE;      // 片选保持高电平的时间
  hqspi.Init.ClockMode          = QSPI_CLOCK_MODE_0;            // 模式3
  hqspi.Init.FlashID            = QSPI_FLASH_ID_1;            // 使用QSPI1
  hqspi.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;       // 禁止双闪存模式
  
  while (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {}
}

int8_t QSPI_W25Qxx_Init(void)
{
  uint32_t  Device_ID;                         // 器件ID
  MX_QUADSPI_Init();                           // 初始化 QSPI 配置
  QSPI_W25Qxx_Reset();                         // 复位器件
  Device_ID = QSPI_W25Qxx_ReadID();            // 读取器件ID
  
  if ( Device_ID == W25Qxx_FLASH_ID )          // 返回成功标志
  { return QSPI_W25Qxx_OK; }
  else                                         // 返回错误标志
  { return W25Qxx_ERROR_INIT; }
}



/**
 * @function     ReadStatusReg
 * @brief        None
 * @param[in]    Reg: [输入/出]
**       mode: [输入/出]
 * @return       None
 * @date         2024-01-03
*/
uint8_t ReadStatusReg(uint8_t Reg, uint8_t mode)
{
  uint8_t RegStatus = 0x00;
  QSPI_CommandTypeDef s_command;
  s_command.Instruction       = Reg;                        // 读取寄存器指令
  s_command.Address           = 0;                          // 无地址
  s_command.DummyCycles       = 0;                          // 空周期个数
  
  if (mode == QspiMode)
  {
    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;   // 4-Line-Instruct
    s_command.DataMode        = QSPI_DATA_4_LINES;          // 4-Line-Dat-Mode
  }
  else if (mode == SpiMode)
  {
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;    // 1-Line-Instruct
    s_command.DataMode        = QSPI_DATA_1_LINE;           // 1-Line-Dat-Mode
  }
  else
  { return -1; }
  
  s_command.AddressMode       = QSPI_ADDRESS_NONE;          // 无地址模式
  s_command.AddressSize       = QSPI_ADDRESS_8_BITS;        // 8位地址模式
  // 以下为公共参数
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;   // 每次传输数据都发送指令
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;      // 禁止DDR模式
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
  
  // 开始发送数据
  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  { return -2; }
  
  // 配置数据长度
  hqspi.Instance->DLR = 0;
  
  // 开始接收数据
  if (HAL_QSPI_Receive(&hqspi, &RegStatus, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  { return -3;}
  
  return RegStatus;
}

/**
 * @function     QspiModeSwitch
 * @brief        None
 * @param[in]    mode: [输入/出]
 * @return       None
 * @date         2024-01-03
*/
int8_t QspiModeSwitch(uint8_t mode)
{
  QSPI_CommandTypeDef s_command;
  s_command.Address           = 0;                          // 无地址
  s_command.DummyCycles       = 0;                          // 空周期个数
  
  if (mode == QspiMode)
  {
    s_command.Instruction     = W25X_EnterQPIMode;          // 进入QSPI模式
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;    // 1-Line-Instruct
  }
  else
  {
    s_command.Instruction     = W25X_ExitQPIMode;           // 退出QSPI模式
    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;   // 4-Line-Instruct
  }
  
  s_command.AddressMode       = QSPI_ADDRESS_NONE;          // 无地址模式
  s_command.DataMode          = QSPI_DATA_NONE;             // 无数据模式
  s_command.AddressSize       = QSPI_ADDRESS_8_BITS;        // 8位地址模式
  // 以下为公共参数
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;   // 每次传输数据都发送指令
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;      // 禁止DDR模式
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
  
  // 开始发送数据
  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  { return -1; }
  
  return 0;
}
