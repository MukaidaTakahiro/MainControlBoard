#include "ExternalBoard.h"

ExternalBoard::ExternalBoard(   const InternalCommunicationPtr bob_comm,
                                const InternalCommunicationPtr prb_comm,
                                const InternalCommunicationPtr eob_comm)
{
    board_comm_list_[BoardId::kBob] = bob_comm;
    board_comm_list_[BoardId::kPrb] = prb_comm;
    board_comm_list_[BoardId::kEob] = eob_comm;
}

ExternalBoard::~ExternalBoard()
{
}


bool ExternalBoard::SendCmd(BoardId dest, 
                            const std::vector<uint8_t> send_cmd,
                            std::vector<uint8_t>* response)
{
    bool result;

    //メッセージ送信
    result = 
        board_comm_list_[dest]->SendInCmdPacket(send_cmd, response);


    return result;
}
