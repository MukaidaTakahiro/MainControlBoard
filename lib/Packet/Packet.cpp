#include "Packet.h"

Packet::Packet()
{
}

Packet::Packet(const std::vector<uint8_t>& command_data)
{
    uint16_t command_size = command_data.size();

    command_data_.reserve(command_size);
    std::copy(command_data.begin(), command_data.end(), command_data_.begin());

    packet_data_.reserve(command_size + kMinPacketSize - 1);
    packet_data_.push_back(kIndexHeader1);
    packet_data_.push_back(kIndexHeader2);
    packet_data_.push_back(packet_data_.max_size());
    std::copy(command_data.begin(), command_data.end(), packet_data_.end());

}
