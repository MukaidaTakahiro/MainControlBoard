#ifndef MOCK_DEVICE_H_
#define MOCK_DEVICE_H_

#include <stdint.h>
#include <vector>

#include "IResetIc.h"

class MockResetIc: public IResetIc
{
public:
    MOCK_METHOD0(ShutdownSystem, bool(void));
};
#endif /* MOCK_DEVICE_H_ */