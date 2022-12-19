#include "MockFreeRtos.h"

freertos::MockFreeRtos MockFreeRtosObj;


/******************************************************************************/
/* タイマー関連 ***************************************************************/
/******************************************************************************/


TimerHandle_t xTimerCreate( const char * const pcTimerName,
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

/******************************************************************************/
/* タスク関連 *****************************************************************/
/******************************************************************************/
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        const char * const pcName,
                        const configSTACK_DEPTH_TYPE usStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pxCreatedTask )
{
    return MockFreeRtosObj.xTaskCreate( pxTaskCode,
                                        pcName,
                                        usStackDepth,
                                        pvParameters,
                                        uxPriority,
                                        pxCreatedTask );
}

void vTaskDelete( TaskHandle_t xTaskToDelete )
{
    return MockFreeRtosObj.vTaskDelete(xTaskToDelete);
}