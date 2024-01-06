/**
   @copyright None
   @file      msp.h
   @author    Comment Vivre
   @date      2023-12-29
   @brief     None
*/
#ifndef __MSP_H_
#define __MSP_H_

#include <main.h>

typedef struct
{
	uint16_t SetWidth;                             // 宽度
	uint16_t SetHeight;                            // 高度
	uint8_t  PixelsSize;                           // 像素大小
	uint8_t  ActiveLayer;                          // 激活层
	uint8_t  ShowDir;                              // 显示方向
}LTDC_Def;

// SDRAM参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

// LCD 屏幕参数
// 正点原子 RGB屏幕 1024*600
#define Width  1024
#define Height 600
#define HBP    140
#define VBP    20
#define HSW    20
#define VSW    3
#define HFP    160
#define VFP    12

#define LCD_Layer_0_Addr    (0xC0000000)       // Layer 0 显存起始位置
#define LCD_Layer_1_Addr    (0xC0258000)       // Layer 1 显存起始位置
#define LVGL_Buff_0_Addr    (0xC04B0000)       // LVGL 缓存区
#define MAX_Memery_Addr     (0xC3FFFFFF)

void Error_Handler(void);
void MPU_Config(void);
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_GPIO_Init(void);
void MX_FMC_Init(void);
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef * hsdram, FMC_SDRAM_CommandTypeDef * Command);
void MX_DMA_Init(void);
void MX_USART1_UART_Init(void);
void MX_LTDC_Init(void);
void MX_DMA2D_Init(void);


#if 0
#define Black     0xFF000000 // 黑色
#define White     0xFFFFFFFF // 白色
#define RED       0xFFFF0000 // 红色
#define GREEN     0xFF00FF00 // 绿色
#define BLUE      0xFF0000FF // 蓝色
#define Yellow    0xFFFFFF00 // 黄色
#define Cyan      0xFF00FFFF // 青色
#define Magenta   0xFFFF00FF // 品红色
#define Gray      0xFF808080 // 灰色
#define Maroon    0xFF800000 // 栗色
#define Olive     0xFF808000 // 橄榄色
#define Navy      0xFF000080 // 海军蓝
#define Purple    0xFF800080 // 紫色
#define Teal      0xFF008080 // 水鸭蓝
#define Silver    0xFFC0C0C0 // 银色
#define DarkRed   0xFF8B0000 // 深红色
#define Brown     0xFFA52A2A // 棕色
#define DarkBlue  0xFF00008B // 深蓝色
#define DarkGreen 0xFF006400 // 深绿色
#define DarkCyan  0xFF008B8B // 深青色
#define DarkGray  0xFFA9A9A9 // 深灰色
#define Orange    0xFFFFA500 // 橙色
#define Gold      0xFFFFD700 // 金色
#define Coral     0xFFFF7F50 // 珊瑚色
#define Lime      0xFF00FF00 // 酸橙色
#define Indigo    0xFF4B0082 // 靛青色
#define Pink      0xFFFFC0CB // 粉红色
#define Lavender  0xFFE6E6FA // 熏衣草紫
#define SkyBlue   0xFF87CEEB // 天蓝色
#define Turquoise 0xFF40E0D0 // 绿松石色
#define Beige     0xFFF5F5DC // 米色
#define Salmon    0xFFFA8072 // 鲑鱼色
#define Khaki     0xFFF0E68C // 卡其色
#define Mint      0xFF98FF98 // 薄荷绿
#define Orchid    0xFFDA70D6 // 兰花紫
#define SlateGray 0xFF708090 // 石板灰
#else
#define Black     0x0000      // 黑色
#define White     0xFFFF      // 白色
#define RED       0xF800      // 红色
#define GREEN     0x07E0      // 绿色
#define BLUE      0x001F      // 蓝色
#define Yellow    0xFFE0      // 黄色
#define Cyan      0x07FF      // 青色
#define Magenta   0xF81F      // 品红色
#define Gray      0x8410      // 灰色
#define Maroon    0x8000      // 栗色
#define Olive     0x8400      // 橄榄色
#define Navy      0x0010      // 海军蓝
#define Purple    0x8010      // 紫色
#define Teal      0x0410      // 水鸭蓝
#define Silver    0xC618      // 银色
#define DarkRed   0x8800      // 深红色
#define Brown     0xA145      // 棕色
#define DarkBlue  0x0011      // 深蓝色
#define DarkGreen 0x0320      // 深绿色
#define DarkCyan  0x0451      // 深青色
#define DarkGray  0x2104      // 深灰色
#define Orange    0xFD20      // 橙色
#define Gold      0xFEA0      // 金色
#define Coral     0xFBEA      // 珊瑚色
#define Lime      0x07E0      // 酸橙色
#define Indigo    0x4810      // 靛青色
#define Pink      0xFC9F      // 粉红色
#define Lavender  0xE73F      // 熏衣草紫
#define SkyBlue   0x867D      // 天蓝色
#define Turquoise 0x471A      // 绿松石色
#define Beige     0xF7BB      // 米色
#define Salmon    0xFC0E      // 鲑鱼色
#define Khaki     0xF731      // 卡其色
#define Mint      0x07FF      // 薄荷绿
#define Orchid    0xDB9A      // 兰花紫
#define SlateGray 0x7412      // 石板灰

#endif

typedef enum
{
  MIN_SIZE = 0,
  SMALL_SIZE = 1,
  LARGE_SIZE = 2,
  MAX_SIZE  = 3
} FontSize;


void LTDC_Draw_Point(uint16_t x, uint16_t y, uint8_t TargetLayer, uint32_t color);
void LTDC_Fill_Area(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);
void LCD_Show_Char(uint16_t x, uint16_t y, FontSize size, uint8_t dat);
void LCD_Show_Chars(uint16_t x, uint16_t y, FontSize size, const char * text);

typedef struct
{
	// 屏幕实际参数
	uint16_t width;
	uint16_t height;
	// 坐标
	uint16_t axis_x[5];
	uint16_t axis_y[5];
	// bit7:5 保留
	// bit4:0 数据有效性
	uint8_t  touch_valid;
	// bit7:1 保留
	// bit0:0 竖屏 1横屏幕
	uint8_t  touch_dir;
}Touch_Def;

// touch moudle

void Touch_Init(void);
void Touch_Scan(void);
void ctp_test(void);

void Message_Out(const char * text,FontSize size);
void display_hex(uint16_t x, uint16_t y, FontSize size, uint32_t num);

void NAND_Init(void);
void Configure_Memory_Protection(void);

#endif


