#ifndef BINARY_BUFFER_H
#define BINARY_BUFFER_H

#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>

class BinaryBuffer
{
public:
    explicit BinaryBuffer(size_t length) : _bitCounter(0), _data(length, 0){}
    explicit BinaryBuffer(std::vector<uint8_t> data) : _data(std::move(data)), _bitCounter(0){}
    BinaryBuffer(uint8_t *data, size_t length) : _data(data, data + length), _bitCounter(0){}
    BinaryBuffer(const BinaryBuffer &msg) = default;
    BinaryBuffer(BinaryBuffer &&msg) noexcept : _data(std::move(msg._data)), _bitCounter(msg._bitCounter){}

    ~BinaryBuffer() = default;

    /**
     * @tparam T Value type
     * @param value Value to write
     * @param writeStart Where to start writing
     * @param length Length to write
     * @param bigEndian order
     * @return bits written
     */
    template<typename T>
    size_t insert(T value, size_t writeStart, size_t length)
    {
        // If passed value requires more bits
        if( _leftmostSetBit(value) > length )
            return 0;

        size_t bitsLeftInByte = 8 - writeStart % 8; ///< bits left in current byte
        size_t bitsLeftToWrite = length; ///< remaining length to write

        while (bitsLeftToWrite > bitsLeftInByte)
        {
            bitsLeftToWrite -= bitsLeftInByte; // We count from right to left (cause bits)

            _data[writeStart / 8] |= _getBits(value, bitsLeftToWrite, bitsLeftInByte);

            writeStart += bitsLeftInByte;
            bitsLeftInByte = 8;
        }

        _data[writeStart / 8] |= _getBits(value, 0, bitsLeftToWrite) << (bitsLeftInByte - bitsLeftToWrite);

        return length;
    }

    /**
     * Writes value to the end of binary message
     * @param value Value to add
     * @param length In bits
     * @return number Added length
     */
    template<typename T>
    size_t append(T value, size_t length)
    {
        size_t pos = insert<T>(value, _bitCounter, length);
        _bitCounter += length;
        return pos;
    }


    size_t resize(size_t bytes)
    {
        _data.resize(bytes);

        // if new size is smaller than current
        if ( bytes * 8 < _bitCounter )
            bytes = 0;

        return bytes;
    }

    /**
     * @return uint_8t* data
     */
    [[maybe_unused]] uint8_t* data() noexcept{return _data.data();};

    /**
     * @return const uint_8t* data
     */
    [[maybe_unused]] const uint8_t* cdata() noexcept{return _data.data();};

    /**
     * Get's value from array
     * @tparam T return type
     * @param start position of the first bit
     * @param length length of value
     * @return value
     */
    template<typename T>
    T get(const size_t& start, size_t length)
    {
        size_t buff = start;
        return get<T>(buff, length);
    }

    /**
     * Get's value from array
     * @tparam T return type
     * @param start position of the first bit
     * @param length length of value
     * @return value
     */
    template<typename T>
    T get(size_t& start, size_t length)
    {
        T number{};
        size_t byte = start / 8;

        uint8_t bitsLeftInByte = 8u - (start % 8u);

        start += length;

        while (length >= bitsLeftInByte)
        {
            T temp = _getBits(_data[byte], 0, bitsLeftInByte);
            temp <<= length - bitsLeftInByte;
            number |= temp;

            length -= bitsLeftInByte;

            bitsLeftInByte = 8;
            ++byte;
        }

        size_t shift = bitsLeftInByte - length;
        number |= _getBits(_data[byte], shift, length);

        return number;
    }

    /**П
     * @return size Current size in bits
     */
    [[maybe_unused]] [[nodiscard]] size_t size() const noexcept{ return _bitCounter; };

    /**
     * Get bits from value without shift (i.e. taken bits would be first)
     * @tparam T type
     * @param value Source
     * @param start start bit
     * @param quantity number of bits
     * @return
     */
    template<typename T>
    inline static T _getBits(T value, size_t start, size_t quantity) noexcept
    {
        unsigned long long mask = (1u << quantity) - 1u;
        return (value & (mask << start)) >> start;
    }

    /**
     * Finds index of leftmost set bit
     * @tparam T input type
     * @param value Where to search
     * @return index (from left (for example for 00(1)00101 it would be 5)
     */
    template<typename T>
    inline static size_t _leftmostSetBit(T value) noexcept
    {
        size_t length = sizeof(T) - 1;

        while ( !_getByte(value, length) && length )
            --length;

        return _leftmostSetBit(_getByte(value, length)) + length * 8;
    }

    /**
     * Returns index of rightmost set bit
     * @tparam T input type
     * @param value input value
     * @return index of rightmost set bit
     */
    template<typename T>
    inline static size_t _rightmostSetBit(T value) noexcept
    {
        return log2(value & -value);
    }

    /**
     * @return true for little, false for big
     */
    [[nodiscard]] static bool _littleEndian() noexcept
    {
        int num = 1;
        return *(char *)&num;
    }

    /**
     * Return n-th byte of value;
     * Endianess-dependent
     * @tparam T input type
     * @param value value to get byte from, value considered to be stored as big-endian
     * @param n number of byte to get
     * @return byte value
     */
    template<typename T>
    inline static uint8_t _getByte(T value, size_t n) noexcept
    {
        return ((uint8_t*)(&value))[n];
    }

    friend std::ostream& operator<<(std::ostream& os, const BinaryBuffer& msg);

    std::vector<uint8_t> _data; ///< Actual data //TODO don't actually need vector, can be changed to uint8_t*
    size_t _bitCounter{}; ///< Last written bit
};

template<>
inline size_t BinaryBuffer::_leftmostSetBit<uint8_t>(uint8_t value) noexcept
{
    size_t pos = 0;
    while (value >>= 1u)
        pos++;

    return pos;
}

std::ostream &operator<<(std::ostream &os, const BinaryBuffer &msg)
{
    for(const auto & b : msg._data)
        os << std::bitset<8>(b) << '\'';
    return os;
}

#endif