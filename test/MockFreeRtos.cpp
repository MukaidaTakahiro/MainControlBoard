#include "MockFreeRtos.h"

freertos::MockFreeRtos MockFreeRtosObj;

TimerHandle_t xTimerCreate(const char * const pcTimerName,
                            const TickType_t xTimerPeriodInTicks,
                            const UBaseType_t uxAutoReload,
                            void * const pvTimerID,
                            TimerCallbackFunction_t pxCallbackFunction)
{
    return MockFreeRtosObj.xTimerCreate(pcTimerName, 
                                        xTimerPeriodInTicks, 
                                        uxAutoReload,
                                        pvTimerID, 
                                        pxCallbackFunction);
}

BaseType_t xTimerGenericCommand(TimerHandle_t xTimer,
                                const BaseType_t xCommandID,
                                const TickType_t xOptionalValue,
                                BaseType_t * const pxHigherPriorityTaskWoken,
                                const TickType_t xTicksToWait )
{
    return MockFreeRtosObj.xTimerGenericCommand(xTimer, 
                                                xCommandID,
                                                xOptionalValue,
                                                pxHigherPriorityTaskWoken,
                                                xTicksToWait );
}

void * pvTimerGetTimerID( const TimerHandle_t xTimer )
{
    return MockFreeRtosObj.pvTimerGetTimerID(xTimer);
}

TickType_t xTaskGetTickCount( void )
{
    return MockFreeRtosObj.xTaskGetTickCount();
}

TickType_t xTaskGetTickCountFromISR( void )
{
    return MockFreeRtosObj.xTaskGetTickCountFromISR();
}