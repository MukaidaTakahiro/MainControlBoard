#ifndef THRUSTER_MGR_H_
#define THRUSTER_MGR_H_


#include <gmock/gmock.h>

#include "IThrusterMgr.h"

class MockThrusterMgr: public IThrusterMgr
{
public:
    MOCK_METHOD1(OperateThruster, bool(const std::vector<uint16_t>));
};

#endif /* THRUSTER_MGR_H_ */