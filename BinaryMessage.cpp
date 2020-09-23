#include "BinaryMessage.h"

BinaryMessage::BinaryMessage(size_t length) : _bitCounter(0), _data(length, 0)
{}

[[maybe_unused]] uint8_t *BinaryMessage::Data() noexcept
{
    return _data.data();
}

[[maybe_unused]] size_t BinaryMessage::Size() const noexcept
{
    return _bitCounter;
}

[[maybe_unused]] char *BinaryMessage::DataInChar()
{
    return reinterpret_cast<char *>( _data.data() );
}