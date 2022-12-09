#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include "HeartBeat.h"
#include <MockFreeRtos.h>
#include <MockNotificationUartIrq.h>

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Pointee;
using ::testing::PrintToString;
using ::testing::Invoke;
using ::testing::SaveArg;
using ::testing::SaveArgPointee;
using ::testing::DoAll;

