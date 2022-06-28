#include "bsp_usart.h"

/**
  * @brief  利用USART1发送字符串
  */
USART1_printf
{
  HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);
  return ch;
}
