#ifndef MOCK_BMS_MGR_H_
#define MOCK_BMS_MGR_H_

#include <gmock/gmock.h>

#include <IBmsMgr.h>

class MockBmsMgr: public IBmsMgr
{
    MOCK_METHOD0(GetBmsParam, BmsParam(void));
};

#endif /* MOCK_BMS_MGR_H_ */