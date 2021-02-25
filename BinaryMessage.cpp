#include "BinaryMessage.hpp"

#include <utility>

BinaryBuffer::BinaryBuffer(size_t length) : _bitCounter(0), _data(length, 0){}

BinaryBuffer::BinaryBuffer(uint8_t *data, size_t length) : _data(data, data + length), _bitCounter(0){}

BinaryBuffer::BinaryBuffer(std::vector<uint8_t> data) : _data(std::move(data)), _bitCounter(0){}

BinaryBuffer::BinaryBuffer(const BinaryBuffer &msg) = default;

BinaryBuffer::BinaryBuffer(BinaryBuffer &&msg) noexcept : _data(std::move(msg._data)), _bitCounter(msg._bitCounter){}

[[maybe_unused]] uint8_t *BinaryBuffer::data() noexcept
{
    return _data.data();
}

[[maybe_unused]] size_t BinaryBuffer::size() const noexcept
{
    return _bitCounter;
}

//TODO get rid of bitset
//TODO add manipulator like std::hex
std::ostream &operator<<(std::ostream &os, const BinaryBuffer &msg)
{
    for(const auto & b : msg._data)
        os << std::bitset<8>(b) << '\'';
    return os;
}