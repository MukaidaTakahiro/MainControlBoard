#ifndef THRUSTER_H_
#define THRUSTER_H_

#include <stdint.h>
#include <vector>

#include "tim.h"


#include "IThruster.h"

class Thruster: public IThruster
{
public:
    Thruster(TIM_HandleTypeDef* const, const uint32_t);
    ~Thruster();
    virtual bool SetDuty(uint32_t);

private:
    TIM_HandleTypeDef* const htim_;
    const uint32_t channel_;
};

#endif /* THRUSTER_H_ */