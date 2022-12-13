#ifndef MOCK_UART_COMMUNICATION_H_
#define MOCK_UART_COMMUNICATION_H_

#include <stdint.h>
#include <gmock/gmock.h>
#include "IUartCommunication.h"

class MockUartCommunication: public IUartCommunication
{
public:
    MOCK_METHOD0(Init, void(void));
    MOCK_METHOD1(WaitReceiveData, bool(const TickType_t));
	MOCK_METHOD0(ReadByte, uint8_t(void));
	MOCK_METHOD0(IsUartEmpty, bool(void));
    MOCK_METHOD0(ClearBuffer, void(void));
	MOCK_METHOD1(SendMsg, bool(std::vector<uint8_t>));
};

#endif /* MOCK_UART_COMMUNICATION_H_ */