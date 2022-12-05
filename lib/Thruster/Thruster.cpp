#include "Thruster.h"

Thruster::Thruster(TIM_HandleTypeDef* const htim, const uint32_t channel)
:   htim_(htim), channel_(channel)
{
}

Thruster::~Thruster()
{
}


bool Thruster::SetDuty(uint32_t duty)
{
    if (htim_ == nullptr)
    {
        return false;
    }

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.Pulse        = duty;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_ConfigChannel(htim_, &sConfigOC, channel_);
    HAL_TIM_PWM_Start(htim_, channel_);
}

