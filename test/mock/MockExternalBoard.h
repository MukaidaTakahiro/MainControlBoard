#ifndef EXTERNAL_BOARD_H_
#define EXTERNAL_BOARD_H_

#include <gmock/gmock.h>

#include "IExternalBoard.h"

class MockExternalBoard: public IExternalBoard
{
public:
    MOCK_METHOD3(SendCmd, bool( BoardId, 
                                const std::vector<uint8_t>, 
                                std::vector<uint8_t>*));
};

#endif /* EXTERNAL_BOARD_H_ */