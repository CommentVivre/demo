#include "main.h"


#if 0
volatile TouchStructure touchInfo;      //  触摸信息结构体，在函数 Touch_Scan() 里被调用，存储触摸数据
volatile static uint8_t Modify_Flag = 0;  // 触摸坐标修改标志位
/*****************************************************************************************
  函 数 名: Touch_IIC_GPIO_Config
  入口参数: 无
  返 回 值: 无
  函数功能: 初始化IIC的GPIO口,推挽输出
  说    明: 由于IIC通信速度不高，这里的IO口速度配置为2M即可
******************************************************************************************/

void Touch_IIC_GPIO_Config (void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  Touch_IIC_SCL_CLK_ENABLE; //初始化IO口时钟
  Touch_IIC_SDA_CLK_ENABLE;
  Touch_INT_CLK_ENABLE;
  Touch_RST_CLK_ENABLE;
  GPIO_InitStruct.Pin       = Touch_IIC_SCL_PIN;        // SCL引脚
  GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_OD;      // 开漏输出
  GPIO_InitStruct.Pull    = GPIO_NOPULL;            // 不带上下拉
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;      // 速度等级
  HAL_GPIO_Init(Touch_IIC_SCL_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin       = Touch_IIC_SDA_PIN;        // SDA引脚
  HAL_GPIO_Init(Touch_IIC_SDA_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;           // 推挽输出
  GPIO_InitStruct.Pull  = GPIO_PULLUP;              // 上拉
  GPIO_InitStruct.Pin = Touch_INT_PIN;              //  INT
  HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = Touch_RST_PIN;              //  RST
  HAL_GPIO_Init(Touch_RST_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_SET);   // SCL输出高电平
  HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_SET);    // SDA输出高电平
  HAL_GPIO_WritePin(Touch_INT_PORT,     Touch_INT_PIN,     GPIO_PIN_RESET);  // INT输出低电平
  HAL_GPIO_WritePin(Touch_RST_PORT,     Touch_RST_PIN,     GPIO_PIN_SET);    // RST输出高电平
}

/*****************************************************************************************
  函 数 名: Touch_IIC_Delay
  入口参数: a - 延时时间
  返 回 值: 无
  函数功能: 简单延时函数
  说    明: 为了移植的简便性且对延时精度要求不高，所以不需要使用定时器做延时
******************************************************************************************/

void Touch_IIC_Delay(uint32_t a)
{
  volatile uint16_t i;
  
  while (a --)
  {
    for (i = 0; i < 8; i++);
  }
}

/*****************************************************************************************
  函 数 名: Touch_IIC_INT_Out
  入口参数: 无
  返 回 值: 无
  函数功能: 配置IIC的INT脚为输出模式
  说    明: 无
******************************************************************************************/

void Touch_INT_Out(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;        // 输出模式
  GPIO_InitStruct.Pull  = GPIO_PULLUP;          // 上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      // 速度等级
  GPIO_InitStruct.Pin   = Touch_INT_PIN ;         // 初始化 INT 引脚
  HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);
}

/*****************************************************************************************
  函 数 名: Touch_IIC_INT_In
  入口参数: 无
  返 回 值: 无
  函数功能: 配置IIC的INT脚为输入模式
  说    明: 无
******************************************************************************************/

void Touch_INT_In(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;          // 输入模式
  GPIO_InitStruct.Pull  = GPIO_NOPULL;          // 浮空
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      // 速度等级
  GPIO_InitStruct.Pin   = Touch_INT_PIN ;         // 初始化 INT 引脚
  HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);
}

/*****************************************************************************************
  函 数 名: Touch_IIC_Start
  入口参数: 无
  返 回 值: 无
  函数功能: IIC起始信号
  说    明: 在SCL处于高电平期间，SDA由高到低跳变为起始信号
******************************************************************************************/

void Touch_IIC_Start(void)
{
  Touch_IIC_SDA(1);
  Touch_IIC_SCL(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SDA(0);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(0);
  Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
  函 数 名: Touch_IIC_Stop
  入口参数: 无
  返 回 值: 无
  函数功能: IIC停止信号
  说    明: 在SCL处于高电平期间，SDA由低到高跳变为起始信号
******************************************************************************************/

void Touch_IIC_Stop(void)
{
  Touch_IIC_SCL(0);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SDA(0);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SDA(1);
  Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
  函 数 名: Touch_IIC_ACK
  入口参数: 无
  返 回 值: 无
  函数功能: IIC应答信号
  说    明: 在SCL为高电平期间，SDA引脚输出为低电平，产生应答信号
******************************************************************************************/

void Touch_IIC_ACK(void)
{
  Touch_IIC_SCL(0);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SDA(0);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(0);   // SCL输出低时，SDA应立即拉高，释放总线
  Touch_IIC_SDA(1);
  Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
  函 数 名: Touch_IIC_NoACK
  入口参数: 无
  返 回 值: 无
  函数功能: IIC非应答信号
  说    明: 在SCL为高电平期间，若SDA引脚为高电平，产生非应答信号
******************************************************************************************/

void Touch_IIC_NoACK(void)
{
  Touch_IIC_SCL(0);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SDA(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(0);
  Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
  函 数 名: Touch_IIC_WaitACK
  入口参数: 无
  返 回 值: 无
  函数功能: 等待接收设备发出应答信号
  说    明: 在SCL为高电平期间，若检测到SDA引脚为低电平，则接收设备响应正常
******************************************************************************************/

uint8_t Touch_IIC_WaitACK(void)
{
  Touch_IIC_SDA(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  Touch_IIC_SCL(1);
  Touch_IIC_Delay(IIC_DelayVaule);
  
  if ( HAL_GPIO_ReadPin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN) != 0) //判断设备是否有做出响应
  {
    Touch_IIC_SCL(0);
    Touch_IIC_Delay( IIC_DelayVaule );
    return ACK_ERR; //无应答
  }
  else
  {
    Touch_IIC_SCL(0);
    Touch_IIC_Delay( IIC_DelayVaule );
    return ACK_OK;  //应答正常
  }
}

/*****************************************************************************************
  函 数 名:  Touch_IIC_WriteByte
  入口参数: IIC_Data - 要写入的8位数据
  返 回 值:  ACK_OK  - 设备响应正常
            ACK_ERR - 设备响应错误
  函数功能: 写一字节数据
  说    明:高位在前
******************************************************************************************/

uint8_t Touch_IIC_WriteByte(uint8_t IIC_Data)
{
  uint8_t i;
  
  for (i = 0; i < 8; i++)
  {
    Touch_IIC_SDA(IIC_Data & 0x80);
    Touch_IIC_Delay( IIC_DelayVaule );
    Touch_IIC_SCL(1);
    Touch_IIC_Delay( IIC_DelayVaule );
    Touch_IIC_SCL(0);
    
    if (i == 7)
    {
      Touch_IIC_SDA(1);
    }
    
    IIC_Data <<= 1;
  }
  
  return Touch_IIC_WaitACK(); //等待设备响应
}

/*****************************************************************************************
  函 数 名:  Touch_IIC_ReadByte
  入口参数: ACK_Mode - 响应模式，输入1则发出应答信号，输入0发出非应答信号
  返 回 值:  ACK_OK  - 设备响应正常
            ACK_ERR - 设备响应错误
  函数功能:读一字节数据
  说    明:1.高位在前
        2.应在主机接收最后一字节数据时发送非应答信号
******************************************************************************************/

uint8_t Touch_IIC_ReadByte(uint8_t ACK_Mode)
{
  uint8_t IIC_Data = 0;
  uint8_t i = 0;
  
  for (i = 0; i < 8; i++)
  {
    IIC_Data <<= 1;
    Touch_IIC_SCL(1);
    Touch_IIC_Delay( IIC_DelayVaule );
    IIC_Data |= (HAL_GPIO_ReadPin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN) & 0x01);
    Touch_IIC_SCL(0);
    Touch_IIC_Delay( IIC_DelayVaule );
  }
  
  if ( ACK_Mode == 1 )        //  应答信号
  { Touch_IIC_ACK(); }
  else
  { Touch_IIC_NoACK(); }      // 非应答信号
  
  return IIC_Data;
}

/********************************************************************************************/
/*************************************************************************************************************************************
  函 数 名:  GT9XX_Reset
  入口参数: 无
  返 回 值:  无
  函数功能: 复位GT911
  说    明: 复位GT911，并将芯片的IIC地址配置为0xBA/0xBB
************************************************************************************************************************************/

void GT9XX_Reset(void)
{
  Touch_INT_Out();  //  将INT引脚配置为输出
  // 初始化引脚状态
  HAL_GPIO_WritePin(Touch_INT_PORT, Touch_INT_PIN, GPIO_PIN_RESET); // INT输出低电平
  HAL_GPIO_WritePin(Touch_RST_PORT, Touch_RST_PIN, GPIO_PIN_SET);  // RST输出高  电平
  Touch_IIC_Delay(10000);
  // 开始执行复位
  //  INT引脚保持低电平不变，将器件地址设置为0XBA/0XBB
  HAL_GPIO_WritePin(Touch_RST_PORT, Touch_RST_PIN, GPIO_PIN_RESET); // 拉低复位引脚，此时芯片执行复位
  Touch_IIC_Delay(150000);      // 延时
  HAL_GPIO_WritePin(Touch_RST_PORT, Touch_RST_PIN, GPIO_PIN_SET);   // 拉高复位引脚，复位结束
  Touch_IIC_Delay(350000);      // 延时
  Touch_INT_In();           // INT引脚转为浮空输入
  Touch_IIC_Delay(20000);       // 延时
}

/*************************************************************************************************************************************
  函 数 名:  GT9XX_WriteHandle
  入口参数: addr - 要操作的寄存器
  返 回 值:  SUCCESS - 操作成功
          ERROR   - 操作失败
  函数功能: GT9XX 写操作
  说    明: 对指定的寄存器执行写操作
************************************************************************************************************************************/

uint8_t GT9XX_WriteHandle (uint16_t addr)
{
  uint8_t status;   // 状态标志位
  Touch_IIC_Start();  // 启动IIC通信
  
  if ( Touch_IIC_WriteByte(GT9XX_IIC_WADDR) == ACK_OK ) //写数据指令
  {
    if ( Touch_IIC_WriteByte((uint8_t)(addr >> 8)) == ACK_OK ) //写入16位地址
    {
      if ( Touch_IIC_WriteByte((uint8_t)(addr)) != ACK_OK )
      {
        status = ERROR; // 操作失败
      }
    }
  }
  
  status = SUCCESS; // 操作成功
  return status;
}

/*************************************************************************************************************************************
  函 数 名:  GT9XX_WriteData
  入口参数: addr - 要写入的寄存器
          value - 要写入的数据
  返 回 值:  SUCCESS - 操作成功
          ERROR   - 操作失败
  函数功能: GT9XX 写一字节数据
  说    明: 对指定的寄存器写入一字节数据
************************************************************************************************************************************/

uint8_t GT9XX_WriteData (uint16_t addr, uint8_t value)
{
  uint8_t status;
  Touch_IIC_Start(); //启动IIC通讯
  
  if ( GT9XX_WriteHandle(addr) == SUCCESS) //写入要操作的寄存器
  {
    if (Touch_IIC_WriteByte(value) != ACK_OK) //写数据
    {
      status = ERROR;
    }
  }
  
  Touch_IIC_Stop(); // 停止通讯
  status = SUCCESS; // 写入成功
  return status;
}

/*************************************************************************************************************************************
  函 数 名:  GT9XX_WriteReg
  入口参数: addr - 要写入的寄存器区域首地址
          cnt  - 数据长度
          value - 要写入的数据区
  返 回 值:  SUCCESS - 操作成功
          ERROR   - 操作失败
  函数功能: GT9XX 写寄存器
  说    明: 往芯片的寄存器区写入指定长度的数据
************************************************************************************************************************************/

uint8_t GT9XX_WriteReg (uint16_t addr, uint8_t cnt, uint8_t * value)
{
  uint8_t status;
  uint8_t i;
  Touch_IIC_Start();
  
  if ( GT9XX_WriteHandle(addr) == SUCCESS)  // 写入要操作的寄存器
  {
    for (i = 0 ; i < cnt; i++)      // 计数
    {
      Touch_IIC_WriteByte(value[i]);  // 写入数据
    }
    
    Touch_IIC_Stop();   // 停止IIC通信
    status = SUCCESS;   // 写入成功
  }
  else
  {
    Touch_IIC_Stop();   // 停止IIC通信
    status = ERROR;   // 写入失败
  }
  
  return status;
}

/*************************************************************************************************************************************
  函 数 名:  GT9XX_ReadReg
  入口参数: addr - 要读取的寄存器区域首地址
          cnt  - 数据长度
          value - 要读取的数据区
  返 回 值:  SUCCESS - 操作成功
          ERROR   - 操作失败
  函数功能: GT9XX 读寄存器
  说    明: 从芯片的寄存器区读取指定长度的数据
************************************************************************************************************************************/

uint8_t GT9XX_ReadReg (uint16_t addr, uint8_t cnt, uint8_t * value)
{
  uint8_t status;
  uint8_t i;
  status = ERROR;
  Touch_IIC_Start();    // 启动IIC通信
  
  if ( GT9XX_WriteHandle(addr) == SUCCESS) //写入要操作的寄存器
  {
    Touch_IIC_Start(); //重新启动IIC通讯
    
    if (Touch_IIC_WriteByte(GT9XX_IIC_RADDR) == ACK_OK) // 发送读命令
    {
      for (i = 0 ; i < cnt; i++) // 计数
      {
        if (i == (cnt - 1))
        {
          value[i] = Touch_IIC_ReadByte(0); // 读到最后一个数据时发送 非应答信号
        }
        else
        {
          value[i] = Touch_IIC_ReadByte(1); // 发送应答信号
        }
      }
      
      Touch_IIC_Stop(); // 停止IIC通信
      status = SUCCESS;
    }
  }
  
  Touch_IIC_Stop(); // 停止IIC通信
  return (status);
}

/*************************************************************************************************************************************
  函 数 名: Touch_Init
  入口参数: 无
  返 回 值: SUCCESS  - 初始化成功
             ERROR   - 错误，未检测到触摸屏
  函数功能: 触摸IC初始化，并读取相应信息发送到串口
  说    明: 初始化触摸面板
************************************************************************************************************************************/

uint8_t Touch_Init(void)
{
  uint8_t GT9XX_Info[11]; // 触摸屏IC信息
  uint8_t cfgVersion = 0; // 触摸配置版本
  Touch_IIC_GPIO_Config();  // 初始化IIC引脚
  GT9XX_Reset();          // 复位IC
  GT9XX_ReadReg (GT9XX_ID_ADDR, 11, GT9XX_Info);  // 读触摸屏IC信息
  GT9XX_ReadReg (GT9XX_CFG_ADDR, 1, &cfgVersion); // 读触摸配置版本
  
  if ( GT9XX_Info[0] == '9' ) //判断第一个字符是否为 9
  { return SUCCESS; }
  else
  { return ERROR; }
}

/*************************************************************************************************************************************
  函 数 名: Touch_Scan
  入口参数: 无
  返 回 值: 无
  函数功能: 触摸扫描
  说    明: 在程序里周期性的调用该函数，用以检测触摸操作，触摸信息存储在 touchInfo 结构体
************************************************************************************************************************************/

void Touch_Scan(void)
{
  uint8_t  touchData[2 + 8 * TOUCH_MAX ];     // 用于存储触摸数据
  GT9XX_ReadReg (GT9XX_READ_ADDR, 2 + 8 * TOUCH_MAX, touchData); // 读数据
  GT9XX_WriteData (GT9XX_READ_ADDR, 0);                 //  清除触摸芯片的寄存器标志位
  touchInfo.num = touchData[0] & 0x0f;                  // 取当前的触摸点数
  
  if ( (touchInfo.num >= 1) && (touchInfo.num <= 5) ) //  当触摸数在 1-5 之间时
  {
    for (uint8_t i = 0; i < touchInfo.num; i++) // 取相应的触摸坐标
    {
      touchInfo.y[i] = (touchData[5 + 8 * i] << 8) | touchData[4 + 8 * i]; // 获取Y坐标
      touchInfo.x[i] = (touchData[3 + 8 * i] << 8) | touchData[2 + 8 * i]; //  获取X坐标
    }
    
    touchInfo.flag = 1; // 触摸标志位置1，代表有触摸动作发生
  }
  else // 触摸标志位置0，无触摸动作
  { touchInfo.flag = 0; }
}


#endif