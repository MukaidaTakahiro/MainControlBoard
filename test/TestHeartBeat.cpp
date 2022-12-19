#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <memory>
#include <functional>
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
    std::unique_ptr<HeartBeat> heart_beat_;
    std::unique_ptr<MockNotificationUartIrq> uart_irq_;
    std::unique_ptr<MockResetIc> reset_ic_;
    TimerHandle_t timer_ = {0};
    void* timeout_callback_instance_;
    TimerCallbackFunction_t timeout_callback_func_;
    void* heartbeat_callback_instance_;
    INotificationUartIrq::NotifyHeartBeatCallback heartbeat_callback_func_;

    virtual void SetUp()
    {
        EXPECT_CALL(MockFreeRtosObj,xTimerCreate(_,_,pdFALSE,_,_))
            .Times(1)
            .WillOnce(DoAll(SaveArg<3>(&timeout_callback_instance_), 
                            SaveArgPointee<4>(&timeout_callback_func_),
                            Return(timer_)));

        uart_irq_ = std::make_unique<MockNotificationUartIrq>();
        reset_ic_ = std::make_unique<MockResetIc>();
        heart_beat_ = std::make_unique<HeartBeat>(*reset_ic_);

        EXPECT_CALL(*uart_irq_, RegistNotifyHeartBeatCallback(_, _))
            .Times(1)
            .WillOnce(DoAll(SaveArg<0>(&heartbeat_callback_instance_),
                            SaveArg<1>(&heartbeat_callback_func_),
                            Return(true)));

        heart_beat_->Init(*uart_irq_);
    }

    virtual void TearDown()
    {
    }
};

TEST(HeartBeat, Constructor_1)
{
    EXPECT_CALL(MockFreeRtosObj,xTimerCreate(_,_,pdFALSE,_,_))
        .Times(1);

    auto uart_irq_ = std::make_unique<MockNotificationUartIrq>();
    auto reset_ic_ = std::make_unique<MockResetIc>();
    auto heart_beat_ = std::make_unique<HeartBeat>(*reset_ic_);
}

/**
 * @brief タイマースタート
 * 
 */
TEST_F(HeartBeatTestFixture, StartMonitoring_1)
{
    const TickType_t test_val = 5;

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
    EXPECT_THAT(heart_beat_->GetTimeout(), (uint16_t)test_val);
}

/**
 * @brief タイマー割込み発生
 *        シャットダウンを実行すること
 * 
 */
TEST_F(HeartBeatTestFixture, Timeout_1)
{
    EXPECT_CALL(MockFreeRtosObj, pvTimerGetTimerID(_))
        .WillOnce(Return(timeout_callback_instance_));
    EXPECT_CALL(*reset_ic_, ShutdownSystem);

    timeout_callback_func_(timer_);
}

/**
 * @brief データ受信コールバック発生
 *        監視タイマーをリセットすること
 * 
 */
TEST_F(HeartBeatTestFixture, TimerReset_1)
{
    EXPECT_CALL(MockFreeRtosObj, xTimerGenericCommand(_,
                                        tmrCOMMAND_RESET_FROM_ISR,
                                        _,
                                        0,
                                        0));
}