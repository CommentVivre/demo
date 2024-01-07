/**
   @copyright None
   @file      lcd.c
   @author    Comment Vivre
   @date      2023-12-29
   @brief     None
*/
#include <main.h>
#include "font.h"
#include "stdio.h"

LTDC_Def LTDC_Dat;

/**
   @function     LTDC_Control
   @brief        打开或关闭LCD
   @param[in]    hltdc: [输入/出]
**       flag: [输入/出]
   @return       None
   @date         2024-01-02
*/
void LTDC_Control(LTDC_HandleTypeDef * hltdc, bool flag)
{
  if (flag)
  { __HAL_LTDC_ENABLE(hltdc); }
  else
  { __HAL_LTDC_DISABLE(hltdc);}
}

/**
   @function     LTDC_Layer_Control
   @brief        打开或关闭指定层
   @param[in]    hltdc: [输入/出]
**       layerx: [输入/出]
**       flag: [输入/出]
   @return       None
   @date         2024-01-02
*/
void LTDC_Layer_Control(LTDC_HandleTypeDef * hltdc, uint8_t layerx, bool flag)
{
  if (flag)
  { __HAL_LTDC_LAYER_ENABLE(hltdc, layerx); }
  else
  { __HAL_LTDC_LAYER_DISABLE(hltdc, layerx);}
  
  __HAL_LTDC_RELOAD_CONFIG(hltdc);
}

/**
   @function     LTDC_Display_Dir
   @brief        设置显示方向
   @param[in]    dir: [输入/出]
   @return       None
   @date         2024-01-02
*/
void LTDC_Display_Dir(uint8_t dir)
{
  LTDC_Dat.ShowDir = dir;
  
  /* 重设显示参数
    横屏与竖屏在显存中是两种坐标系
    横屏
    (0,0)      ------> (Width,0)
    |                      |
    |                      |
    (0,Height) ------> (Width,Height) */
  if (dir)
  {
    LTDC_Dat.SetWidth = Width;
    LTDC_Dat.SetHeight = Height;
  }
  else
  {
    LTDC_Dat.SetWidth = Height;
    LTDC_Dat.SetHeight = Width;
  }
}

/**
   @function     LTDC_Draw_Point
   @brief        画点
   @param[in]    x: [输入/出]
**       y: [输入/出]
**       TargetLayer: [输入/出]
**       color: [输入/出]
   @return       None
   @date         2024-01-02
*/
void LTDC_Draw_Point(uint16_t x, uint16_t y, uint8_t TargetLayer, uint32_t color)
{
  if (LTDC_Dat.ShowDir)
  {
    if (TargetLayer)
    { *(uint32_t *)(LCD_Layer_1_Addr + 2 * (Width * x + y)) = color;}
    else
    { *(uint32_t *)(LCD_Layer_0_Addr + 2 * (Width * x + y)) = color;}
  }
  else
  {
    if (TargetLayer)
    { *(uint32_t *)(LCD_Layer_1_Addr + 2 * (Width * (Height - x - 1) + y)) = color;}
    else
    { *(uint32_t *)(LCD_Layer_0_Addr + 2 * (1024 * (600 - x - 1) + y)) = color;}
  }
}

/**
   @function     LTDC_Read_Point
   @brief        读点
   @param[in]    x: [输入/出]
**       y: [输入/出]
**       TargetLayer: [输入/出]
   @return       None
   @date         2024-01-02
*/
uint32_t LTDC_Read_Point(uint16_t x, uint16_t y, uint8_t TargetLayer)
{
  if (LTDC_Dat.ShowDir)
  {
    if (TargetLayer)
    { return *(uint32_t *)(LCD_Layer_1_Addr + 2 * (Width * x + y));}
    else
    { return*(uint32_t *)(LCD_Layer_0_Addr + 2 * (Width * x + y));}
  }
  else
  {
    if (TargetLayer)
    { return*(uint32_t *)(LCD_Layer_1_Addr + 2 * (Width * (Height - x - 1) + y));}
    else
    { return*(uint32_t *)(LCD_Layer_0_Addr + 2 * (Width * (Height - x - 1) + y));}
  }
}

/**
   @brief 使用DMA2D在屏幕上填充矩形区域
   @param sx 矩形区域起始点 x 坐标
   @param sy 矩形区域起始点 y 坐标
   @param ex 矩形区域结束点 x 坐标
   @param ey 矩形区域结束点 y 坐标
   @param color 填充颜色
*/
void LTDC_Fill_Area(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
  uint16_t ActualSX, ActualSY, ActualEX, ActualEY;
  uint32_t timeout = 0;
  
  // 根据显示方向计算矩形区域的实际坐标
  if (LTDC_Dat.ShowDir)
  {
    ActualSX = sx; ActualSY = sy;
    ActualEX = ex; ActualEY = ey;
  }
  else
  {
    ActualSX = sy; ActualSY = Height - ex - 1;
    ActualEX = ey; ActualEY = Height - sx - 1;
  }
  
  // 配置DMA2D参数
  RCC->AHB1ENR |= 1 << 23;
  DMA2D->CR = 3 << 16;
  DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
  DMA2D->OOR = Width - (ActualEX - ActualSX + 1);
  DMA2D->CR &= ~(1 << 0);
  DMA2D->OMAR = (uint32_t)(LCD_Layer_0_Addr) + 4 * (Width * ActualSY + ActualSX);
  DMA2D->NLR = (ActualEY - ActualSY + 1) | ((ActualEX - ActualSX + 1) << 16);
  DMA2D->OCOLR = color;
  DMA2D->CR |= 1 << 0;
  
  // 等待DMA2D传输完成
  while ((DMA2D->ISR & (1 << 1)) == 0)
  {
    timeout++;
    
    if (timeout > 0X1FFFFF)
    {
      break;
    }
  }
  
  DMA2D->IFCR |= 1 << 1; // 清除传输完成标志位
}


extern uint32_t Pen_Color;
extern uint32_t Back_Color;
uint8_t Ascii_Size[4][2] = {{12, 12}, {16, 16}, { 24, 36}, {32, 128}};


/**
   @function     Select_Font
   @brief        根据字符大小返回对应的指针
   @param[in]    offset: [输入/出]
**       size: [输入/出]
   @return       None
   @date         2024-01-05
*/
uint8_t * Select_Font(uint8_t offset, FontSize size)
{
  switch (size)
  {
    case MIN_SIZE: return (uint8_t *)&asc2_1206[offset][0];
    
    case SMALL_SIZE: return (uint8_t *)&asc2_1608[offset][0];
    
    case LARGE_SIZE: return (uint8_t *)&asc2_2412[offset][0];
    
    case MAX_SIZE: return (uint8_t *)&asc2_3216[offset][0];
    
    default: return NULL;
  }
}

/**
   @function     LCD_Show_Char
   @brief        None
   @param[in]    x: [输入/出]
**       y: [输入/出]
**       size: [输入/出]
**       dat: [输入/出]
   @return       None
   @date         2024-01-05
*/
void LCD_Show_Char(uint16_t x, uint16_t y, FontSize size, uint8_t dat)
{
  uint8_t offset = dat - ' ';
  uint8_t * Ascii_Font = Select_Font(offset, size);
  const uint16_t Original_Location = y;
  uint8_t Matrix_Port = 0x00;
  
  if (Ascii_Font == NULL)
  {
    return;
  }
  
  for (uint8_t i = 0; i < Ascii_Size[size][1]; ++i)
  {
    Matrix_Port = *Ascii_Font++;
    
    // 绘图
    for (uint8_t j = 0; j < 8; ++j)
    {
      if (Matrix_Port & 0x80)
      { LTDC_Draw_Point(x, y, 0, Pen_Color); }
      
      Matrix_Port <<= 1;
      ++y;
      // 待添加Y轴限制
      
      if ((y - Original_Location) == Ascii_Size[size][0])
      {
        y = Original_Location;
        ++x;
        // 待添加X轴限制
        break;
      }
    }
  }
}

/**
   @function     LCD_Show_Chars
   @brief        None
   @param[in]    x: [输入/出]
**       y: [输入/出]
**       size: [输入/出]
**       text: [输入/出]
   @return       None
   @date         2024-01-05
*/

void LCD_Show_Chars(uint16_t x, uint16_t y, FontSize size, const char * text)
{
  uint8_t charIndex = 0;
  uint8_t charToShow;
  
  while ((charToShow = text[charIndex++]) != '\0')
  {
    // 检查字符是否在有效范围内
    uint8_t offset = charToShow - ' ';
    uint8_t * font = Select_Font(offset, size);
    
    // 如果字符无效（即字库中没有对应的字符），不显示
    if (font != NULL)
    {
      LCD_Show_Char(x, y, size, charToShow);
      //这里将 x 增加对应的像素的宽度 以便显示下一个字符
      x += Ascii_Size[size][0];
    }
  }
}


/**
 * @brief 在屏幕上输出消息，并在需要时清空屏幕
 *
 * @param text 要显示的文本内容
 */
// 静态变量用于跟踪垂直方向上的文本输出位置
static uint16_t y = 0;

void Message_Out(const char * text, FontSize size)
{
  // 检查 y 值是否超出屏幕范围
  if (y > 1024)
  {
    y = 0;
    // 清空屏幕
    LTDC_Fill_Area(0, 0, 599, 1023, White);
  }
  
  // 在屏幕上显示文本
  LCD_Show_Chars(12, y, size, text);
  // 增加垂直方向上的位置
  y += Ascii_Size[size][0];
}

/**
  * @brief Display a hexadecimal number on the LCD screen.
  * @param x: The x-coordinate for displaying the text on the LCD.
  * @param y: The y-coordinate for displaying the text on the LCD.
  * @param size: The font size for the displayed text.
  * @param num: The hexadecimal number to be displayed.
  * @note This function converts the hexadecimal number to a string and then displays it using LCD_Show_Chars.
  */
void display_hex(uint16_t x, uint16_t y, FontSize size, uint32_t num)
{
  char hex_string[11];  // Assuming a 32-bit number with "0x" prefix and null terminator
  // Format the number as a hexadecimal string
  sprintf(hex_string, "0x%08lX", (unsigned long)num);
  // Display the hexadecimal string on the LCD
  LCD_Show_Chars(x, y, size, hex_string);
}
