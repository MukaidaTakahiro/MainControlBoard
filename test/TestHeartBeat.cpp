#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <memory>
#include <MockFreeRtos.h>

#include "HeartBeat.h"
#include <MockNotificationUartIrq.h>
#include <MockResetIc.h>

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

extern freertos::MockFreeRtos MockFreeRtosObj;

class HeartBeatTestFixture: public::testing::Test
{
protected:
    std::shared_ptr<HeartBeat> heart_beat_;
    std::shared_ptr<MockNotificationUartIrq> uart_irq_;
    std::shared_ptr<MockResetIc> reset_ic_;

    virtual void SetUp()
    {
        EXPECT_CALL(MockFreeRtosObj,xTimerCreate(_,_,pdFALSE,_,_))
        .Times(1);

        uart_irq_ = std::make_shared<MockNotificationUartIrq>();
        reset_ic_ = std::make_shared<MockResetIc>();
        heart_beat_ = std::make_shared<HeartBeat>(uart_irq_, reset_ic_);
    }

    virtual void TearDown()
    {
    }
};

TEST(HeartBeat, Constructor_1)
{
    EXPECT_CALL(MockFreeRtosObj,xTimerCreate(_,_,pdFALSE,_,_))
    .Times(1);

    auto uart_irq_ = std::make_shared<MockNotificationUartIrq>();
    auto reset_ic_ = std::make_shared<MockResetIc>();
    auto heart_beat_ = std::make_shared<HeartBeat>(uart_irq_, reset_ic_);
}

/**
 * @brief タイマースタート
 * 
 */
TEST_F(HeartBeatTestFixture, StartMonitoring_1)
{
    const TickType_t test_val = 5;

    EXPECT_CALL(*uart_irq_, RegistNotifyHeartBeatCallback(_, _))
    .Times(1);

    EXPECT_CALL(MockFreeRtosObj, xTaskGetTickCount())
    .Times(1)
    .WillRepeatedly(Return(test_val));

    EXPECT_CALL(MockFreeRtosObj, 
                xTimerGenericCommand(_, tmrCOMMAND_START, test_val, NULL, 0))
    .Times(1);

    EXPECT_TRUE(heart_beat_->StartMonitoring());
}

/**
 * @brief タイマースタート 2回実施
 * 
 */
TEST_F(HeartBeatTestFixture, StartMonitoring_2)
{
    const TickType_t test_val = 5;

    EXPECT_CALL(*uart_irq_, RegistNotifyHeartBeatCallback(_, _))
    .Times(1);

    EXPECT_CALL(MockFreeRtosObj, xTaskGetTickCount())
    .Times(1)
    .WillRepeatedly(Return(test_val));

    EXPECT_CALL(MockFreeRtosObj, 
                xTimerGenericCommand(_, tmrCOMMAND_START, test_val, NULL, 0))
    .Times(1);

    EXPECT_TRUE(heart_beat_->StartMonitoring());
    EXPECT_FALSE(heart_beat_->StartMonitoring());
}

/**
 * @brief タイマースタート UART割込み登録失敗
 * 
 */
TEST(HeartBeat, StartMonitoring_3)
{
    EXPECT_CALL(MockFreeRtosObj,xTimerCreate(_,_,pdFALSE,_,_))
    .Times(1);

    auto heart_beat_ = std::make_shared<HeartBeat>(nullptr, nullptr);

    EXPECT_FALSE(heart_beat_->StartMonitoring());
}

/**
 * @brief タイマーストップ 
 * 
 */
TEST_F(HeartBeatTestFixture, StopMonitoring_1)
{

    EXPECT_CALL(MockFreeRtosObj, xTaskGetTickCount())
    .Times(1);
    EXPECT_CALL(MockFreeRtosObj, 
                xTimerGenericCommand(_, tmrCOMMAND_START, _, NULL, 0))
    .Times(1);
    EXPECT_CALL(MockFreeRtosObj,
                xTimerGenericCommand(_, tmrCOMMAND_STOP, 0U, NULL, 0))
    .Times(1);

    EXPECT_TRUE(heart_beat_->StartMonitoring());
    EXPECT_TRUE(heart_beat_->StopMonitoring());
}

/**
 * @brief タイマーストップ ストップ状態
 * 
 */
TEST_F(HeartBeatTestFixture, StopMonitoring_2)
{

    EXPECT_CALL(MockFreeRtosObj,
                xTimerGenericCommand(_, tmrCOMMAND_STOP, 0U, NULL, 0))
    .Times(0);

    EXPECT_FALSE(heart_beat_->StopMonitoring());
}

/**
 * @brief タイムアウト変更
 * 
 */
TEST_F(HeartBeatTestFixture, SetMonitoringTimeout)
{
    const uint32_t test_val = 1000;

    EXPECT_CALL(MockFreeRtosObj,
                xTimerGenericCommand(_, tmrCOMMAND_STOP, 0U, NULL, 0))
    .Times(1);

    EXPECT_CALL(
        MockFreeRtosObj,
        xTimerGenericCommand(_, tmrCOMMAND_CHANGE_PERIOD, 
                                test_val / portTICK_PERIOD_MS, NULL, 0))
    .Times(1);

    heart_beat_->SetMonitoringTimeout(test_val);
}