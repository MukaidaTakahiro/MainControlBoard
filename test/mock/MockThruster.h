#ifndef MOCK_THRUSTER_H_
#define MOCK_THRUSTER_H_

#include <gmock/gmock.h>
#include <vector>
#include "IThruster.h"

class MockThruster : public IThruster
{
public:
    MockThruster()
    {}
    ~MockThruster()
    {}
    
    MOCK_METHOD1(OperateThruster, bool(std::vector<int16_t>));
};

#endif /* MOCK_THRUSTER_H_ */