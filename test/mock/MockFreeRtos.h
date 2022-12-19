#ifndef MOCK_FREE_RTOS_H_
#define MOCK_FREE_RTOS_H_

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include <gmock\gmock.h>

namespace freertos
{
class IFreeRtos
{
public:
    virtual ~IFreeRtos() {}

    /* タイマー関連 */
    virtual TimerHandle_t xTimerCreate( 
                                const char * const pcTimerName,
                                const TickType_t xTimerPeriodInTicks,
                                const UBaseType_t uxAutoReload,
                                void * const pvTimerID,
                                TimerCallbackFunction_t pxCallbackFunction) = 0;
    

    virtual BaseType_t xTimerGenericCommand(
                            TimerHandle_t xTimer,
                            const BaseType_t xCommandID,
                            const TickType_t xOptionalValue,
                            BaseType_t * const pxHigherPriorityTaskWoken,
                            const TickType_t xTicksToWait ) = 0;

    virtual void * pvTimerGetTimerID( const TimerHandle_t xTimer ) = 0;

    virtual TickType_t xTaskGetTickCount( void ) = 0;
    virtual TickType_t xTaskGetTickCountFromISR( void ) = 0;
    
    /* タスク関連 */
    virtual BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName,
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask ) = 0;
    virtual void vTaskDelete( TaskHandle_t xTaskToDelete ) = 0;
};

class MockFreeRtos: public IFreeRtos
{
public:
    virtual ~MockFreeRtos() {}

    MOCK_METHOD5(xTimerCreate, TimerHandle_t(const char * const,
                                                const TickType_t,
                                                const UBaseType_t,
                                                void * const,
                                                TimerCallbackFunction_t));
    MOCK_METHOD5(xTimerGenericCommand, BaseType_t(
                                            TimerHandle_t,
                                            const BaseType_t,
                                            const TickType_t,
                                            BaseType_t * const,
                                            const TickType_t));
    MOCK_METHOD1(pvTimerGetTimerID, void *(const TimerHandle_t));
    MOCK_METHOD0(xTaskGetTickCount, TickType_t(void));
    MOCK_METHOD0(xTaskGetTickCountFromISR, TickType_t(void));

    /* タスク関連 */
    MOCK_METHOD6(xTaskCreate, BaseType_t(   TaskFunction_t,
                                            const char * const,
                                            const configSTACK_DEPTH_TYPE,
                                            void * const,
                                            UBaseType_t,
                                            TaskHandle_t * const));
    MOCK_METHOD1(vTaskDelete, void(TaskHandle_t));

};


} // namespace freertos

#endif /* MOCK_FREE_RTOS_H_ */