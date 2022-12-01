#ifndef THRUSTER_H_
#define THRUSTER_H_

#include <stdint.h>
#include <vector>


#include "IThruster.h"

class Thruster:public IThruster
{
public:
    virtual bool OperateThruster(std::vector<int16_t>);
};

#endif /* THRUSTER_H_ */