#include "Thruster.h"



Thruster::Thruster(TIM_HandleTypeDef* const htim, const uint32_t channel)
:   htim_(htim), channel_(channel)
{
    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.Pulse        = 0;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_ConfigChannel(htim_, &sConfigOC, channel_);
    HAL_TIM_PWM_Start(htim_, channel_);
}

Thruster::~Thruster()
{
}

/**
 * @brief PWM値を出力
 *        パルス幅をus単位で扱うため、STM32の設定でPrescalerを調整しておく
 *        (Prescaler) = (APB1 clocks[Hz]) / [1MHz]
 * 
 * @param pulse_width パルス幅[us]
 * @return bool 
 */
bool Thruster::SetPulseWidth(uint32_t pulse_width)
{
    if (htim_ == nullptr)
    {
        return false;
    }

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.Pulse        = pulse_width;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(htim_, &sConfigOC, channel_) != HAL_OK)
    {
        return false;
    }

    HAL_TIM_PWM_Stop(htim_, channel_);
    HAL_TIM_PWM_Start(htim_, channel_);

    return true;
}

