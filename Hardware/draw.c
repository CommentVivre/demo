/**
   @copyright None
   @file      draw.c
   @author    Comment Vivre
   @date      2024-01-05
   @brief     None
*/
#include "main.h"
#include <stdlib.h>

extern uint32_t Pen_Color;
extern uint32_t Back_Color;
//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
  LTDC_Fill_Area(0, 0, 599, 1023, 0xFFFFFFFF);
  Pen_Color = Gray; //设置字体为蓝色
  LCD_Show_Chars(600 - 24, 0, SMALL_SIZE, "RST"); //显示清屏区域
  Pen_Color = White; //设置画笔蓝色
}

/**
   @brief 绘制水平线
   @param x0 起始点 x 坐标
   @param y0 起始点 y 坐标
   @param len 线条长度
   @param color 线条颜色
*/
void gui_draw_hline(uint16_t x0, uint16_t y0, uint16_t len, uint16_t color)
{
  if (len == 0)
  {
    return;  // 长度为0，无需绘制
  }
  
  // 限制坐标范围
  if ((x0 + len - 1) >= 600)
  {
    x0 = 600 - len;  // 修正起始点，确保线条在屏幕内
  }
  
  if (y0 >= 1024)
  {
    y0 = 1024 - 1;  // 修正起始点，确保线条在屏幕内
  }
  
  LTDC_Fill_Area(x0, y0, x0 + len - 1, y0, color); // 绘制水平线
}


/**
   @brief 绘制实心圆
   @param x0 圆心 x 坐标
   @param y0 圆心 y 坐标
   @param r 半径
   @param color 颜色
*/
void gui_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color)
{
  uint16_t x = r;
  uint16_t y = 0;
  uint16_t radiusError = 1 - x;
  
  while (x >= y)
  {
    // 从内到外画圆边缘的线条
    gui_draw_hline(x0 - x, y0 + y, 2 * x, color);
    gui_draw_hline(x0 - y, y0 - x, 2 * y, color);
    gui_draw_hline(x0 - x, y0 - y, 2 * x, color);
    gui_draw_hline(x0 - y, y0 + x, 2 * y, color);
    y++;
    
    if (radiusError < 0)
    { radiusError += 2 * y + 1; }
    else
    {
      x--;
      radiusError += 2 * (y - x + 1);
    }
  }
}

/**
   @brief 在屏幕上绘制一条粗线
   @param x1 起始点 x 坐标
   @param y1 起始点 y 坐标
   @param x2 结束点 x 坐标
   @param y2 结束点 y 坐标
   @param size 线条的粗细程度
   @param color 线条的颜色
*/
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint32_t color)
{
  if (x1 < size || x2 < size || y1 < size || y2 < size)
  {
    return; // 线条粗度大于坐标范围，无法绘制
  }
  
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int incx = (x2 > x1) ? 1 : -1;
  int incy = (y2 > y1) ? 1 : -1;
  
  if (dx >= dy)
  {
    int d = (dy << 1) - dx;
    int incrE = dy << 1;
    int incrNE = (dy - dx) << 1;
    
    for (int x = x1, y = y1, distance = dx; distance >= 0; x += incx, distance--)
    {
      gui_fill_circle(x, y, size, color); // 画点
      
      if (d <= 0)
      {
        d += incrE;
      }
      else
      {
        d += incrNE;
        y += incy;
      }
    }
  }
  else
  {
    int d = (dx << 1) - dy;
    int incrE = dx << 1;
    int incrNE = (dx - dy) << 1;
    
    for (int x = x1, y = y1, distance = dy; distance >= 0; y += incy, distance--)
    {
      gui_fill_circle(x, y, size, color); // 画点
      
      if (d <= 0)
      {
        d += incrE;
      }
      else
      {
        d += incrNE;
        x += incx;
      }
    }
  }
}


extern Touch_Def Touch_dat;

const uint32_t POINT_COLOR_TBL[5] = {0xFFF0F0F0, 0xFFFF00FF, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF};


/**
   @brief 用于触摸屏测试的函数
*/
void ctp_test(void)
{
  uint8_t i = 0; // 计数变量，用于控制LED的翻转
  uint16_t lastpos[5][2] = {{0}}; // 存储每个触摸点的最后位置
  
  while (1)
  {
    Touch_Scan(); // 扫描触摸屏输入
    
    // 迭代每个触摸点
    for (uint8_t touchIndex = 0; touchIndex < 5; touchIndex++)
    {
      if ((Touch_dat.touch_valid) & (1 << touchIndex))   // 检查触摸点是否有效
      {
        // 检查触摸点是否在屏幕范围内
        if (Touch_dat.axis_x[touchIndex] < 1024 && Touch_dat.axis_y[touchIndex] < 600)
        {
          uint16_t x = Touch_dat.axis_x[touchIndex];
          uint16_t y = Touch_dat.axis_y[touchIndex];
          
          // 如果触摸点的上一次位置无效，则设置当前位置为最后位置
          if (lastpos[touchIndex][0] == 0XFFFF)
          {
            lastpos[touchIndex][0] = x;
            lastpos[touchIndex][1] = y;
          }
          
          // 画线
          lcd_draw_bline(lastpos[touchIndex][0], lastpos[touchIndex][1], x, y, 2, POINT_COLOR_TBL[touchIndex]);
          // 更新最后位置
          lastpos[touchIndex][0] = x;
          lastpos[touchIndex][1] = y;
          
          // 检查触摸点是否在指定位置，并执行相应操作
          if (x > (1024 - 24) && y < 20)
          { Load_Drow_Dialog(); }
        }
      }
      else
      {
        // 触摸点无效，将其最后位置标记为无效
        lastpos[touchIndex][0] = 0XFFFF;
      }
    }
    
    HAL_Delay(20); // 延时
    
    // 每 20 次循环翻转 LED 灯的状态
    if (++i % 20 == 0)
    { LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0); }
  }
}
