/**
 * @file ResetIc.cpp
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief 
 * @version 0.1
 * @date 2022-12-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ResetIc.h"

ResetIc::ResetIc(GPIO_TypeDef* const gpio_port, const uint32_t gpio_pin)
:   gpio_port_(gpio_port), gpio_pin_(gpio_pin)
{
    HAL_GPIO_WritePin(gpio_port_, gpio_pin_, GPIO_PIN_RESET);
}

ResetIc::~ResetIc()
{
}

void ResetIc::ShutdownSystem()
{
    HAL_GPIO_WritePin(gpio_port_, gpio_pin_, GPIO_PIN_SET);
}