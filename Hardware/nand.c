/**
 * @copyright None
 * @file      nand.c
 * @author    Comment Vivre
 * @date      2024-01-06
 * @brief     None
 */
#include "main.h"
#include "stdio.h"
#define NAND_MAX_PAGE_SIZE      4096    // 定义NAND FLASH的最大的PAGE大小（不包括SPARE区），默认4096字节
#define NAND_ECC_SECTOR_SIZE    512    // 执行ECC计算的单元大小，默认512字节

// NAND FLASH操作相关延时函数
#define NAND_TADL_DELAY         35    // tADL等待延迟,最少70ns
#define NAND_TWHR_DELAY         30    // tWHR等待延迟,最少60ns
#define NAND_TRHW_DELAY         50    // tRHW等待延迟,最少100ns
#define NAND_TPROG_DELAY        350   // tPROG等待延迟,典型值200us,最大需要700us
#define NAND_TBERS_DELAY        4     // tBERS等待延迟,典型值3.5ms,最大需要10ms
#define NAND_TRST_FIRST_DELAY   3     // tRST复位后的第一次等待时间，最大为3ms

#define NAND_RB                 HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6) // NAND Flash的闲/忙引脚

#define NAND_ADDRESS            0x80000000    // nand flash的访问地址,接NCE3,地址为:0x8000 0000
#define NAND_CMD                1 << 16       // 发送命令
#define NAND_ADDR               1 << 17       // 发送地址

// NAND FLASH命令
#define NAND_READID             0x90    // 读ID指令
#define NAND_FEATURE            0xEF    // 设置特性指令
#define NAND_RESET              0xFF    // 复位NAND
#define NAND_READSTA            0x70    // 读状态
#define NAND_AREA_A             0x00
#define NAND_AREA_TRUE1         0x30
#define NAND_WRITE0             0x80
#define NAND_WRITE_TURE1        0x10
#define NAND_ERASE0             0x60
#define NAND_ERASE1             0xD0
#define NAND_MOVEDATA_CMD0      0x00
#define NAND_MOVEDATA_CMD1      0x35
#define NAND_MOVEDATA_CMD2      0x85
#define NAND_MOVEDATA_CMD3      0x10

// NAND FLASH状态
#define NSTA_READY              0x40    // nand已经准备好
#define NSTA_ERROR              0x01    // nand错误
#define NSTA_TIMEOUT            0x02    // 超时
#define NSTA_ECC1BITERR         0x03    // ECC 1bit错误
#define NSTA_ECC2BITERR         0x04    // ECC 2bit以上错误

// NAND FLASH型号和对应的ID号
#define MT29F4G08ABADA          0xDC909556    // MT29F4G08ABADA
#define MT29F16G08ABABA         0x48002689    // MT29F16G08ABABA

void NAND_Delay(__IO uint32_t i)
{
  while (i > 0)
  {  i--; }
}

/**
 * @brief 从NAND读取状态值
 * @retval 读取的状态值
 */
uint8_t NAND_ReadDeviceStatus(void)
{
  __IO uint8_t data = 0;
  // 发送读状态命令并等待tWHR后读取状态寄存器
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_READSTA;
  NAND_Delay(NAND_TWHR_DELAY);
  // 读取状态值
  data = *(__IO uint8_t *)NAND_ADDRESS;
  return data;
}

/**
 * @brief 等待NAND设备准备就绪
 * @note 如果超时未就绪，返回NSTA_TIMEOUT
 * @retval 就绪状态（NSTA_READY）或超时（NSTA_TIMEOUT）
 */
uint8_t NAND_WaitForDeviceReady(void)
{
  uint8_t status = 0;
  __IO uint32_t time = 0;
  
  // 循环等待 ready 状态
  while (!(status & NSTA_READY))
  {
    status = NAND_ReadDeviceStatus(); // 获取状态值
    
    if (++time >= 0x1FFFF)
    {
      return NSTA_TIMEOUT;  // 超时
    }
  }
  
  return NSTA_READY;  // 准备好
}

/**
 * @brief 设置NAND设备模式
 * @param mode 要设置的模式
 * @note 设置特性命令，并根据模式设置参数
 * @retval 设置成功返回0，失败返回1
 */
uint8_t NAND_SetDeviceMode(uint8_t mode)
{
  // 发送设置特性命令，并设置 mode
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_FEATURE;
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = 0x01;
  // 等待 tADL
  NAND_Delay(NAND_TADL_DELAY);
  // 设置 mode，清空剩余的地址位
  *(__IO uint8_t *)NAND_ADDRESS = mode;
  
  for (uint8_t i = 0; i < 3; ++i)
  { *(__IO uint8_t *)NAND_ADDRESS = 0; }
  
  // 返回状态
  return (NAND_WaitForDeviceReady() == NSTA_READY) ? 0 : 1;
}

/**
 * @brief 从NAND设备读取ID并返回处理后的32位ID值
 * @note 针对镁光NAND FLASH，只取后四个字节组成32位ID值
 * @retval 32位ID值，如果读取失败则返回0xFFFFFFFF
 */
uint32_t NAND_GetDeviceID(void)
{
  uint8_t deviceid[5];
  uint32_t id;
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_READID; // 发送读取ID命令
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = 0x00;
  // NOP指令用于CPU空转延时，共60ns, tWHR
  NAND_Delay(NAND_TWHR_DELAY);
  
  // 读取5个字节的ID
  for (int i = 0; i < 5; ++i)
  { deviceid[i] = *(__IO uint8_t *)NAND_ADDRESS; }
  
  // 针对镁光的NAND FLASH，只取后四字节的ID值，组成32位ID
  id = ((uint32_t)deviceid[1]) << 24 | ((uint32_t)deviceid[2]) << 16 | ((uint32_t)deviceid[3]) << 8 | deviceid[4];
  
  if (NAND_WaitForDeviceReady() == NSTA_READY)
  { return id; }
  else
  { return 0xFFFFFFFF; }
}

/**
 * @brief 复位NAND设备，并等待其就绪
 * @note 镁光NAND设备需要100ns的tWB+100us的tRST
 * @retval 复位成功返回0，失败返回1
 */
uint8_t NAND_DeviceReset(void)
{
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_RESET; // 复位NAND
  // 镁光需要等100ns的tWB+100us的tRST
  HAL_Delay(NAND_TRST_FIRST_DELAY);
  
  if (NAND_WaitForDeviceReady() == NSTA_READY)
  { return 0; }  // 复位成功
  else
  { return 1; }  // 复位失败
}

/**
  * @brief 等待 RB 信号就绪。
  * @param rb: 0，等待 RB 为低电平；1，等待 RB 为高电平。
  * @retval 0 表示成功，1 表示超时。
  */
uint8_t NAND_WaitReadyRB(__IO uint8_t rb)
{
  __IO uint32_t time = 0;
  __IO uint8_t cnt = 0;
  
  while (time < 0X1FFFFFF)
  {
    time++;
    
    if (NAND_RB == rb)
    { cnt++; }
    else
    {  cnt = 0; }
    
    // 连续三次读取都是正确的有效电平，认为此次数据有效！（否则 -O2 优化出问题！）
    if (cnt > 2)
    { return 0;   }
  }
  
  return 1;
}


uint32_t nand_id = 0x0000000;

void NAND_Init(void)
{
  NAND_DeviceReset();
  HAL_Delay(1000);
  nand_id = NAND_GetDeviceID();
  display_hex(10, 10, SMALL_SIZE, nand_id);
  NAND_SetDeviceMode(4);
}


/**
  * @brief 发送NAND Flash的地址信息
  * @param PageNum 要读取的页地址，范围:0~(block_pagenum*block_totalnum-1)
  * @param ColNum 要读取的列开始地址(也就是页内地址)，范围:0~(page_totalsize-1)
  * @param cmd1 第一个命令的选择
  * @param cmd2 第二个命令的选择
  * @retval None
  */
void NAND_SendAddress(uint32_t PageNum, uint16_t ColNum, uint8_t cmd1, uint8_t cmd2)
{
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_CMD) = cmd1;
  // 发送列地址
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)ColNum;
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(ColNum >> 8);
	// 发送页地址
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)PageNum;
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 8);
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(PageNum >> 16);
  *(__IO uint8_t *)(NAND_ADDRESS | NAND_CMD) = cmd2;
}

