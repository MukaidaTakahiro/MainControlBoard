#include "ExternalBoard.h"

ExternalBoard::ExternalBoard(   IInternalCommunication& bob_comm,
                                IInternalCommunication& prb_comm,
                                IInternalCommunication& eob_comm)
:   bob_comm_(bob_comm), 
    prb_comm_(prb_comm),
    eob_comm_(eob_comm)
{
}

ExternalBoard::~ExternalBoard()
{
}


bool ExternalBoard::SendCmd(BoardId dest, 
                            const std::vector<uint8_t> send_cmd,
                            std::vector<uint8_t>& response)
{
    bool result = false;

    switch (dest)
    {
    case BoardId::kBob:
        result = bob_comm_.SendInCmdPacket(send_cmd, response);
        break;
    case BoardId::kPrb:
        result = prb_comm_.SendInCmdPacket(send_cmd, response);
        break;
    case BoardId::kEob:
        result = eob_comm_.SendInCmdPacket(send_cmd, response);
        break;
    }

    return result;
}
