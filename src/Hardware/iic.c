/**
   @copyright None
   @file      iic.c
   @author    Comment Vivre
   @date      2024-01-04
   @brief     None
*/
#include "main.h"

//#define IIC_SCL(n) (n?LL_GPIO_SetOutputPin(GPIOH,GPIO_PIN_6):LL_GPIO_ResetOutputPin(GPIOH,GPIO_PIN_6))
//#define IIC_SDA(n) (n?LL_GPIO_SetOutputPin(GPIOG,GPIO_PIN_7):LL_GPIO_ResetOutputPin(GPIOG,GPIO_PIN_7))
//#define READ_SDA   LL_GPIO_IsInputPinSet(GPIOG,GPIO_PIN_7)
#define IIC_SCL(n) (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_RESET))//SCL
#define IIC_SDA(n) (n?HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET))//SDA  
#define READ_SDA   HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_7)//输入SDA 

// 通信线需要开漏输出
// GT_SCL  PH6
// GT_SDA  PG7
//
// GT_INT  PH7
// GT_RST  PI8
/**
   @function     IIC_Init
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void IIC_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG);
  // PH6
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  // PG7
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  LL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/**
   @function     IIC_Delay
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-05
*/
void IIC_Delay(void)
{
  uint8_t a = 40;
  
  while (a--)
  {
    for (uint8_t i = 0; i < 8; i++);
  }
}

/**
   @function     IIC_Start
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void IIC_Start(void)
{
  //START:when CLK is high,DATA change form high to low
  IIC_SDA(1);                  // 数据线拉高
  IIC_SCL(1);                  // 时钟线拉高
  IIC_Delay();                 // 延时
  IIC_SDA(0);                  // 数据线拉低
  IIC_Delay();                 // 延时
  IIC_SCL(0);                  // 时钟线拉低
  IIC_Delay();                 // 延时
}

/**
   @function     IIC_Stop
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void IIC_Stop(void)
{
  //STOP:when CLK is high DATA change form low to high
  IIC_SDA(0);                   // 数据线拉低
  IIC_Delay();                  // 延时
  IIC_SCL(1);                   // 时钟线拉高
  IIC_Delay();                  // 延时
  IIC_SDA(1);                   // 数据线拉高
  IIC_Delay();                  // 延时
}

/**
   @function     IIC_Wait_Ack
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
bool IIC_Wait_Ack(void)
{
  uint8_t ucErrTime = 0;
  bool rack = true;
  IIC_SDA(1);                   // 数据线拉高
  IIC_Delay();                  // 延时
  IIC_SCL(1);                   // 时钟线拉高
  IIC_Delay();                  // 延时
  
  while (READ_SDA)              // 读取数据引脚
  {
    ucErrTime++;
    
    if (ucErrTime > 250)
    {
      IIC_Stop();               // 发送停止信号
      rack = false;
      break;
    }
  }
  
  IIC_SCL(0);                   // 时钟线拉低
  IIC_Delay();                  // 延时
  return true;
}

/**
   @function     IIC_Ack
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void IIC_Ack(void)
{
  IIC_SDA(0);                   // 数据线拉低
  IIC_Delay();                  // 延时
  IIC_SCL(1);                   // 时钟线拉高
  IIC_Delay();                  // 延时
  IIC_SCL(0);                   // 时钟线拉低
  IIC_Delay();                  // 延时
  IIC_SDA(1);                   // 数据线拉高
  IIC_Delay();                  // 延时
}

/**
   @function     IIC_NAck
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void IIC_NAck(void)
{
  IIC_SDA(1);                   // 数据线拉低
  IIC_Delay();                  // 延时
  IIC_SCL(1);                   // 时钟线拉高
  IIC_Delay();                  // 延时
  IIC_SCL(0);                   // 时钟线拉低
  IIC_Delay();                  // 延时
}

/**
   @function     IIC_Send_Byte
   @brief        None
   @param[in]    dat: [输入/出]
   @return       None
   @date         2024-01-04
*/
void IIC_Send_Byte(uint8_t dat)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    IIC_SDA((dat & 0x80) >> 7); // 数据线写入数据最高位
    IIC_Delay();                // 延时
    IIC_SCL(1);                 // 时钟线拉高
    IIC_Delay();                // 延时
    IIC_SCL(0);                 // 时钟线拉低
    dat <<= 1;                  // 数据左移一位
  }
  
  IIC_SDA(1);                   // 数据线拉高
}

/**
   @function     IIC_Read_Byte
   @brief        None
   @param[in]    ack: [输入/出]
   @return       None
   @date         2024-01-04
*/
uint8_t IIC_Read_Byte(bool ack)
{
  uint8_t dat = 0;
  
  for (uint8_t i = 0; i < 8; i++)
  {
    dat <<= 1;                  // 数据左移一位
    IIC_SCL(1);                 // 时钟线拉高
    IIC_Delay();                // 延时
    
    if (READ_SDA)               // 读取状态
    { dat++; }                  // 写入数据
    
    IIC_SCL(0);                 // 时钟线拉低
    IIC_Delay();                // 延时
  }
  
  if (ack)
  { IIC_Ack(); }                // 发送ACK信号
  else
  { IIC_NAck(); }               // 不发送ACK信号
  
  return dat;
}

