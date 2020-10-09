#include "BinaryMessage.h"

#include <utility>

BinaryMessage::BinaryMessage(size_t length) : _bitCounter(0), _data(length, 0){}

BinaryMessage::BinaryMessage(uint8_t *data, size_t length) : _data(data, data+length), _bitCounter(0){}

BinaryMessage::BinaryMessage(std::vector<uint8_t> data) : _data(std::move(data)), _bitCounter(0){}

BinaryMessage::BinaryMessage(const BinaryMessage &msg) = default;

BinaryMessage::BinaryMessage(BinaryMessage &&msg) noexcept : _data(std::move(msg._data)), _bitCounter(msg._bitCounter){}

[[maybe_unused]] uint8_t *BinaryMessage::Data() noexcept
{
    return _data.data();
}

[[maybe_unused]] size_t BinaryMessage::Size() const noexcept
{
    return _bitCounter;
}

//TODO get rid of bitset
//TODO add manipulator like std::hex
std::ostream &operator<<(std::ostream &os, const BinaryMessage &msg)
{
    for(const auto & b : msg._data)
        os << std::bitset<8>(b) << '\'';
    return os;
}