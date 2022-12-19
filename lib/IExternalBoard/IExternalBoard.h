#ifndef I_EXTERNAL_BOARD_
#define I_EXTERNAL_BOARD_

#include <stdint.h>
#include <memory>

class IExternalBoard
{
public:

    enum class BoardId;
    virtual bool SendCmd(   BoardId,
                            const std::vector<uint8_t>, 
                            std::vector<uint8_t>&) = 0;

};

enum class IExternalBoard::BoardId
{
    kBob = 0,   /* Basic Option Board     */
    kPrb,       /* Power Root Board       */
    kEob        /* Extention Option Board */
};



#endif /* I_EXTERNAL_BOARD_ */