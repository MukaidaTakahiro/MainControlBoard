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
            = std::function<void(std::shared_ptr<void>, std::vector<uint8_t>)>;
    using NotifyPacketSyntaxErrCallbackEntry 
            = std::function<void(std::shared_ptr<void>)>;
    using NotifyChecksumErrCallbackEntry 
            = std::function<void(std::shared_ptr<void>)>;

    virtual ~IExternalCommunication()
    {}
    virtual bool Init() = 0;
    virtual bool RegistNotifyCallback(  std::shared_ptr<void>, 
                                        NotifyRecvCmdCallbackEntry,
                                        NotifyPacketSyntaxErrCallbackEntry,
                                        NotifyChecksumErrCallbackEntry) 
                                        = 0;
    virtual bool SendCmdPacket(std::vector<uint8_t>) = 0;

};


#endif /* I_EXTERNAL_COMMUNICATION_H_ */