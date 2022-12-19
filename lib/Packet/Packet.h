
#include <stdint.h>
#include <vector>

class Packet
{
public:
    enum class PacketParsingResult;

    Packet();
    Packet(const std::vector<uint8_t>&);

    PacketParsingResult PickOutPacket(const std::vector<uint8_t>&);

private:

    /* 定数宣言 */
                                /* 最小パケットサイズ                         */
                                /* header1+header2+パケット長+cmd値+checksum  */
    static constexpr int16_t kMinPacketSize = 5;
                                /* パケット最大サイズ                         */
    static constexpr int16_t kPacketMaxSize = 255;
                                /* 最大コマンドサイズ                         */
    static constexpr int16_t kMaxCmdSize = kPacketMaxSize - kMinPacketSize + 1;

    static constexpr uint8_t kPacketHeader1 = 0xFF; /* header-1 */
    static constexpr uint8_t kPacketHeader2 = 0x00; /* header-2 */

    static constexpr uint16_t kIndexHeader1 = 0;
    static constexpr uint16_t kIndexHeader2 = 1;
    static constexpr uint16_t kIndexLength = 2;


    std::vector<uint8_t> packet_data_;
    std::vector<uint8_t> command_data_;

};

