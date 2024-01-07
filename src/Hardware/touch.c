/**
   @copyright None
   @file      touch.c
   @author    Comment Vivre
   @date      2024-01-04
   @brief     None
*/
#include "main.h"
#include "stdio.h"
const static uint16_t GTIC_AXIS_Table[5] =
{ 0x8150, 0x8158, 0x8160, 0x8168, 0x8170};
Touch_Def Touch_dat;

#if 1
// 触摸IC型号GT911

// I2C读写命令
#define GT_CMD_WR     0x28      //写命令
#define GT_CMD_RD     0x29      //读命令

// GT9147 部分寄存器定义
#define GT_CTRL_REG   0x8040    //GT9147控制寄存器
#define GT_CFGS_REG   0x8047    //GT9147配置起始地址寄存器
#define GT_CHECK_REG  0x80FF    //GT9147校验和寄存器
#define GT_PID_REG    0x8140    //GT9147产品ID寄存器

#define GT_GSTID_REG  0x814E    //GT9147当前检测到的触摸情况
#define GT_TP1_REG    0x8150    //第一个触摸点数据地址
#define GT_TP2_REG    0x8158    //第二个触摸点数据地址
#define GT_TP3_REG    0x8160    //第三个触摸点数据地址
#define GT_TP4_REG    0x8168    //第四个触摸点数据地址
#define GT_TP5_REG    0x8170    //第五个触摸点数据地址


// 模拟IIC函数声明
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
bool IIC_Wait_Ack(void);
void IIC_Ack(void); void IIC_NAck(void);
void IIC_Send_Byte(uint8_t dat);
uint8_t IIC_Read_Byte(bool ack);


/**
   @function     GTIC_Weite_Reg
   @brief        None
   @param[in]    reg: [输入/出]
**       dat: [输入/出]
**       len: [输入/出]
   @return       None
   @date         2024-01-04
*/
bool GTIC_Weite_Reg(uint16_t reg, uint8_t * dat, uint8_t len)
{
  bool ack = true;
  // 写入操作寄存器
  IIC_Start();
  IIC_Send_Byte(GT_CMD_WR);
  IIC_Wait_Ack();
  IIC_Send_Byte((reg >> 8) & 0x00FF);
  IIC_Wait_Ack();
  IIC_Send_Byte(reg & 0x00FF);
  IIC_Wait_Ack();
  
  // 写入数据
  for (uint8_t i = 0; i < len; i++)
  {
    IIC_Send_Byte(*dat);
    dat++;
    
    // 等待ACK信号
    if (!IIC_Wait_Ack())
    {
      ack = false;
      break;
    }
  }
  
  IIC_Stop();
  return ack;
}

/**
   @function     GTIC_Read_Reg
   @brief        None
   @param[in]    reg: [输入/出]
**       dat: [输入/出]
**       len: [输入/出]
   @return       None
   @date         2024-01-04
*/
void GTIC_Read_Reg(uint16_t reg, uint8_t * dat, uint8_t len)
{
  // 写入操作寄存器
  IIC_Start();
  IIC_Send_Byte(GT_CMD_WR);
  IIC_Wait_Ack();
  IIC_Send_Byte((reg >> 8) & 0x00FF);
  IIC_Wait_Ack();
  IIC_Send_Byte(reg & 0x00FF);
  IIC_Wait_Ack();
  // 开始读数据
  IIC_Start();
  IIC_Send_Byte(GT_CMD_RD);
  IIC_Wait_Ack();
  
  for (uint8_t i = 0; i < len; i++)
  {
    // 最后一个数据发送ACK信号
    *dat = IIC_Read_Byte((i == (len - 1)) ? 0 : 1);
    dat++;
  }
  
  IIC_Stop();
}

uint8_t CTIC_DAT[4] = {0};
char GTIC_ID[8] = {0};
/**
   @function     Touch_Init
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void Touch_Init(void)
{
  uint8_t dat[4] = {0x00};
  // 初始化引脚
  IIC_Init();
  // 读取芯片ID
  GTIC_Read_Reg(GT_PID_REG, CTIC_DAT, 4);
  sprintf(GTIC_ID, "%d%d%d%d", CTIC_DAT[0], CTIC_DAT[1], CTIC_DAT[2], CTIC_DAT[3]);
  LCD_Show_Chars(10, 10, SMALL_SIZE, GTIC_ID);
  // 复位芯片
  dat[0] = 0x02;
  GTIC_Weite_Reg(GT_CTRL_REG, dat, 1);
  // 读取配置寄存器
  GTIC_Read_Reg(GT_CFGS_REG, dat, 1);
  HAL_Delay(10);
  // 结束复位 开始读取坐标
  dat[0] = 0x00;
  GTIC_Weite_Reg(GT_CTRL_REG, dat, 1);
}

/**
   @function     touch_scan
   @brief        None
   @param[in]    None
   @return       None
   @date         2024-01-04
*/
void Touch_Scan(void)
{
  uint8_t dat[4] = {0};
  uint8_t pen_sum;
  // 读取触摸状态
  GTIC_Read_Reg(GT_GSTID_REG, dat, 1);
  pen_sum = dat[0] & 0x0F;
  
  // 触摸点数正常 且缓冲区状态置位 清除该寄存器
  if ((dat[0] & 0x80) && (pen_sum <= 5))
  { GTIC_Weite_Reg(GT_GSTID_REG, &dat[3], 1); }
  
  // 无触摸
  if ((dat[0] & 0x8F) == 0x80)
  {
    if (Touch_dat.touch_valid & 0x80)
    { Touch_dat.touch_valid &= ~0x80; }
    else
    {
      Touch_dat.axis_x[0] = 0xFFFF;
      Touch_dat.axis_y[0] = 0xFFFF;
      Touch_dat.touch_valid &= 0xE0;
    }
  }
  // 有触摸 读回坐标
  else if (pen_sum && (pen_sum <= 5))
  {
    // 写入有效性
    Touch_dat.touch_valid = (~(0xFF << pen_sum)) | 0x80;
    HAL_Delay(4);
    
    // 判断数据有效性
    for (uint8_t i = 0; (Touch_dat.touch_valid & (0x01 << i)) && (i < 5); i++)
    {
      // 读取坐标数据
      GTIC_Read_Reg(GTIC_AXIS_Table[i], dat, 4);
      
      // 横屏
      if (Touch_dat.touch_dir)
      {
        Touch_dat.axis_x[i] = (uint16_t)((dat[1] << 8) + dat[0]);
        Touch_dat.axis_y[i] = (uint16_t)((dat[3] << 8) + dat[2]);
      }
      // 竖屏
      else
      {
        Touch_dat.axis_x[i] = Width - (uint16_t)((dat[3] << 8) + dat[2]);
        Touch_dat.axis_y[i] = (uint16_t)((dat[1] << 8) + dat[0]);
      }
      
      #if 0
      
      // 判断数据所处范围
      // 非法数据 清空有效性
      if (Touch_dat.axis_x[i] > Touch_dat.width || (Touch_dat.axis_y[i] > Touch_dat.height))
      { Touch_dat.touch_valid &= (~(0x01 << i)); }
      
      #endif
    }
  }
}
#endif
