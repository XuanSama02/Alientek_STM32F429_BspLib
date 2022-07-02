#include "bsp_f429_led.h"

/**
 * @brief //控制F429开发板板载LED
 * 
 * @param LED_Color LED颜色(LED_COLOR_RED/LED_COLOR_GREEN)
 * @param LED_Status LED状态(LED_ON/LED_OFF)
 */
void F429_LED(bool LED_Color, bool LED_Status)
{
    if(LED_Color == LED_COLOR_RED)
    {
        if(LED_Status == LED_ON)
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    }
    else
    {
        if(LED_Status == LED_ON)
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
    }
}

/**
 * @brief //翻转F429开发板板载LED
 * 
 * @param LED_Color LED颜色(LED_COLOR_RED/LED_COLOR_GREEN)
 */
void F429_LED_Toggle(bool LED_Color)
{
    if(LED_Color == LED_COLOR_RED)
        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    else
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}
