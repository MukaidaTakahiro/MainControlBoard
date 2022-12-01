#ifndef INTERNAL_COMMUNICATION_H_
#define INTERNAL_COMMUNICATION_H_

#include <stdint.h>
#include <map>
#include <tuple>
#include <queue>
#include <memory>
#include "FreeRTOS.h"
#include "timers.h"

#include "IInternalCommunication.h"
#include "IUartCommunication.h"
#include "TaskBase.h"

class InternalCommunication
:   public IInternalCommunication, public TaskBase
{
public:


    InternalCommunication(std::shared_ptr<IUartCommunication>);
    ~InternalCommunication();

    virtual bool SendInCmdPacket(   const std::vector<uint8_t>,
                                    std::unique_ptr<std::vector<uint8_t>>);

private:

    static constexpr uint32_t kCommTimeout    = 500;
    static constexpr uint16_t kReceiveTimeout = 1000;
    static constexpr uint16_t kMinPacketSize  = 5;
    static constexpr uint8_t  kPacketHeader1  = 0xFF; /* header-1 */
    static constexpr uint8_t  kPacketHeader2  = 0x00; /* header-2 */
    static constexpr uint16_t kHeader1Index   = 0;
    static constexpr uint16_t kHeader2Index   = 1;
    static constexpr uint16_t kLengthIndex    = 2;
    
    enum class ParsingState
    {
        kWaitingHeader1,       /* header1取得待ち       */
        kWaitingHeader2,       /* header2取得待ち       */
        kWaitingPakcketLength, /* パケット長取得待ち    */
        kGettingCmdData,       /* コマンドデータ取得中  */
        kWaitingCheckSum,      /* チェックサム          */
        kCompleteParsing        /* 解析完了 */ 
    };

    enum class ParsingResult
    {
        kParsing,
        kComplete,
        kMismatch
    };
    
    TimerHandle_t receive_timer_;

    /* 変数宣言 */
    std::shared_ptr<IUartCommunication> uart_comm_;
    bool IsReceiveTimeout;


    /* 関数宣言 */
    ParsingResult ParsePacket(const std::vector<uint8_t>);
    std::vector<uint8_t> GetPacket();
    static void TimeoutCallbackEntry(TimerHandle_t);
    void HandleTimeout();
    std::vector<uint8_t> MakePacket(const std::vector<uint8_t>);
    uint8_t AddCheckSum(const uint8_t, const uint8_t);
    bool ReceiveResponse(const std::unique_ptr<std::vector<uint8_t>>);
    uint8_t CalcChecksum(const std::vector<uint8_t>);

};


#endif /* INTERNAL_COMMUNICATION_H_ */