#ifndef RESET_IC_H_
#define RESET_IC_H_

#include <stdint.h>
#include <gpio.h>

#include "IResetIc.h"

class ResetIc: public IResetIc
{
public:
    ResetIc(GPIO_TypeDef *,uint32_t);
    ~ResetIc();

    virtual void ShutdownSystem();

private:
    GPIO_TypeDef* const gpio_port_;
    const uint32_t gpio_pin_;
};


#endif /* RESET_IC_H_ */