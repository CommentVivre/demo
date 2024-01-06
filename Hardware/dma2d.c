/**
   @copyright None
   @file      dma2d.c
   @author    Comment Vivre
   @date      2023-12-30
   @brief     None
*/
#include <main.h>

/**
   @function     MX_DMA2D_Init
   @brief        None
   @param[in]    None
   @return       None
   @date         2023-12-30
*/
void MX_DMA2D_Init(void)
{
  /* Peripheral clock enable */
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_DMA2D);
  LL_DMA2D_SetMode(DMA2D, LL_DMA2D_MODE_M2M);
  LL_DMA2D_SetOutputColorMode(DMA2D, LL_DMA2D_OUTPUT_MODE_ARGB8888);
  LL_DMA2D_SetLineOffset(DMA2D, 0);
  LL_DMA2D_FGND_SetColorMode(DMA2D, LL_DMA2D_INPUT_MODE_ARGB8888);
  LL_DMA2D_FGND_SetAlphaMode(DMA2D, LL_DMA2D_ALPHA_MODE_NO_MODIF);
  LL_DMA2D_FGND_SetAlpha(DMA2D, 0);
  LL_DMA2D_FGND_SetLineOffset(DMA2D, 0);
  LL_DMA2D_FGND_SetRBSwapMode(DMA2D, LL_DMA2D_RB_MODE_REGULAR);
  LL_DMA2D_FGND_SetAlphaInvMode(DMA2D, LL_DMA2D_ALPHA_REGULAR);
  LL_DMA2D_FGND_SetChrSubSampling(DMA2D, LL_DMA2D_CSS_444);
}
