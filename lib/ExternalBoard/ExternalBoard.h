/**
 * @file ExternalBoard.h
 * @author Mukaida Takahiro (takahiro.mukaida@fulldepth.co.jp)
 * @brief 外部ボードとのコマンド通信を担う
 *        外部ボードからのコマンドの解析を担う
 * @version 0.1
 * @date 2022-11-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef EXTERNAL_BOARD_
#define EXTERNAL_BOARD_

#include <stdint.h>
#include <memory>

#include "IInternalCommunication.h"
#include "IExternalBoard.h"

class ExternalBoard: public IExternalBoard
{
public:

    ExternalBoard(  IInternalCommunication&,
                    IInternalCommunication&,
                    IInternalCommunication&);
    ~ExternalBoard();

    virtual bool SendCmd(   BoardId,
                            const std::vector<uint8_t>, 
                            std::vector<uint8_t>&);

private:

    IInternalCommunication& bob_comm_;
    IInternalCommunication& prb_comm_;
    IInternalCommunication& eob_comm_;
};

#endif /* EXTERNAL_BOARD_ */