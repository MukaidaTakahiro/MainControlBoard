#ifndef I_EXTERNAL_COMMUNICATION_H_
#define I_EXTERNAL_COMMUNICATION_H_

#include <stdint.h>
#include <vector>
#include <functional>
#include <memory>

class IExternalCommunication
{
public:
    using NotifyRecvCmdCallbackEntry 
            = std::function<void(void*, std::vector<uint8_t>)>;
    using NotifyPacketSyntaxErrCallbackEntry 
            = std::function<void(void*)>;
    using NotifyChecksumErrCallbackEntry 
            = std::function<void(void*)>;

    virtual ~IExternalCommunication()
    {}
    virtual bool Init() = 0;
    virtual bool RegistNotifyCallback(  void*, 
                                        NotifyRecvCmdCallbackEntry,
                                        NotifyPacketSyntaxErrCallbackEntry,
                                        NotifyChecksumErrCallbackEntry) 
                                        = 0;
    virtual bool SendCmdPacket(std::vector<uint8_t>) = 0;

};


#endif /* I_EXTERNAL_COMMUNICATION_H_ */