#include "bsp_delay_tim.h"

/**
 * @brief 微秒级延时函数(支持RTOS)
 * 
 * @param nus 微秒数
 */
void Delay_us(uint16_t nus)
{
    uint16_t Conter = 0xFFFF-nus-5;
    __HAL_TIM_SET_COUNTER(&htim10, Conter);  //设置重装寄存器
    HAL_TIM_Base_Start(&htim10);
    while(Conter < 0xFFFF-6)
        Conter = __HAL_TIM_GET_COUNTER(&htim10);
    HAL_TIM_Base_Stop(&htim10);
}
