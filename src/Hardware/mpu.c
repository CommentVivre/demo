#include "main.h"

/**
  * @brief Configure MPU protection for a specific region.
  * @param baseaddr: Base address of the protected area.
  * @param size: Size of the protected area (must be a multiple of 32 bytes).
  * @param rnum: Region number (0 to 7, maximum 8 regions).
  * @param ap: Access permissions (reference values: MPU_REGION_NO_ACCESS, MPU_REGION_PRIV_RW, MPU_REGION_PRIV_RW_URO, MPU_REGION_FULL_ACCESS, MPU_REGION_PRIV_RO, MPU_REGION_PRIV_RO_URO).
  * @param sen: Shareability (MPU_ACCESS_NOT_SHAREABLE or MPU_ACCESS_SHAREABLE).
  * @param cen: Cacheability (MPU_ACCESS_NOT_CACHEABLE or MPU_ACCESS_CACHEABLE).
  * @param ben: Bufferability (MPU_ACCESS_NOT_BUFFERABLE or MPU_ACCESS_BUFFERABLE).
  * @retval 0 if successful, otherwise an error code.
  */
uint8_t Configure_MPU_Protection(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben)
{
  MPU_Region_InitTypeDef MPU_Initure;
  HAL_MPU_Disable();  // Disable MPU before configuration, enable it after configuration
  MPU_Initure.Enable = MPU_REGION_ENABLE;  // Enable the protection region
  MPU_Initure.Number = rnum;  // Set the protection region number
  MPU_Initure.BaseAddress = baseaddr;  // Set the base address
  MPU_Initure.Size = size;  // Set the protection area size
  MPU_Initure.SubRegionDisable = 0x00;  // Disable sub-regions
  MPU_Initure.TypeExtField = MPU_TEX_LEVEL0;  // Set the type extension field to level 0
  MPU_Initure.AccessPermission = (uint8_t)ap;  // Set the access permissions
  MPU_Initure.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;  // Allow instruction access (read instructions)
  MPU_Initure.IsShareable = sen;  // Set shareability
  MPU_Initure.IsCacheable = cen;  // Set cacheability
  MPU_Initure.IsBufferable = ben;  // Set bufferability
  HAL_MPU_ConfigRegion(&MPU_Initure);  // Configure MPU
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);  // Enable MPU
  return 0;
}

/**
  * @brief Configure MPU protection for specific memory blocks.
  * @note Certain memory areas must be protected using MPU to avoid program malfunction, such as display errors or camera data issues.
  */
void Configure_Memory_Protection(void)
{
  // Protect entire D1 SRAM 512KB
  Configure_MPU_Protection(0x24000000,                   // Base address
    MPU_REGION_SIZE_512KB,        // Length
    MPU_REGION_NUMBER1,           // Region number 1
    MPU_REGION_FULL_ACCESS,       // Full access
    MPU_ACCESS_SHAREABLE,         // Shareable
    MPU_ACCESS_CACHEABLE,         // Cacheable
    MPU_ACCESS_NOT_BUFFERABLE);   // Not bufferable
  // Protect SDRAM area, 64MB total
  Configure_MPU_Protection(0xC0000000,                   // Base address
    MPU_REGION_SIZE_64MB,         // Length
    MPU_REGION_NUMBER2,           // Region number 2
    MPU_REGION_FULL_ACCESS,       // Full access
    MPU_ACCESS_NOT_SHAREABLE,     // Not shareable
    MPU_ACCESS_CACHEABLE,         // Cacheable
    MPU_ACCESS_BUFFERABLE);       // Bufferable
  // Configure MPU for NAND Flash
  Configure_MPU_Protection(0x80000000,                   // Base address
    MPU_REGION_SIZE_512MB,        // Length
    MPU_REGION_NUMBER4,           // Region number 4
    MPU_REGION_FULL_ACCESS,       // Full access
    MPU_ACCESS_NOT_SHAREABLE,     // Not shareable
    MPU_ACCESS_NOT_CACHEABLE,     // Not cacheable
    MPU_ACCESS_NOT_BUFFERABLE);   // Not bufferable
}
