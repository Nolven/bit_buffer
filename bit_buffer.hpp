#pragma once

#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>
#include <sstream>

class BitBuffer
{
public:
    explicit BitBuffer() = default;
    explicit BitBuffer(size_t length) :  _data(length, 0){}
    explicit BitBuffer(std::vector<uint8_t> data) : _data(std::move(data)){}
    BitBuffer(uint8_t *data, size_t length) : _data(data, data + length){}
    BitBuffer(const BitBuffer &buffer) = default;
    BitBuffer(BitBuffer &&buffer) noexcept : _data(std::move(buffer._data)), _bitCounter(buffer._bitCounter){}

    /**
     * @tparam T Value type
     * @param value Value to write
     * @param writeStart Starting bit in existing data
     * @param length Number of bits to write
     * @return number of set bits
     */
    template<typename T>
    size_t insert(T value, size_t writeStart, size_t length)
    {
        if( _leftmostSetBit(value) > length )
            return 0;

        size_t bitsLeftInByte = CHAR_BIT - writeStart % CHAR_BIT; ///< bits left in current byte
        size_t bitsLeftToWrite = length; ///< remaining length to write

        while (bitsLeftToWrite > bitsLeftInByte)
        {
            bitsLeftToWrite -= bitsLeftInByte; // We count from right to left (cause bits)
            auto byte = writeStart / CHAR_BIT;
            if( byte > _data.size() - 1 ) _data.template emplace_back(0);
            _data[byte] |= getBits(value, bitsLeftToWrite, bitsLeftInByte);

            writeStart += bitsLeftInByte;
            bitsLeftInByte = CHAR_BIT;
        }

        if( writeStart / CHAR_BIT > _data.size() - 1 ) _data.template emplace_back(0);
        _data[writeStart / CHAR_BIT] |= getBits(value, 0, bitsLeftToWrite) << (bitsLeftInByte - bitsLeftToWrite);

        return length;
    }

    template<typename T>
    size_t append(T value, size_t length)
    {
        size_t pos = insert<T>(value, _bitCounter, length);
        _bitCounter += length;
        return pos;
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
        size_t byte = start / CHAR_BIT;

        uint8_t bitsLeftInByte = CHAR_BIT - (start % CHAR_BIT);

        start += length;

        while (length >= bitsLeftInByte)
        {
            T temp = getBits(_data[byte], 0, bitsLeftInByte);
            temp <<= length - bitsLeftInByte;
            number |= temp;

            length -= bitsLeftInByte;

            bitsLeftInByte = CHAR_BIT;
            ++byte;
        }

        size_t shift = bitsLeftInByte - length;
        number |= getBits(_data[byte], shift, length);

        return number;
    }


    [[nodiscard]] std::string string(const std::string& byteDelim = "") const {
        std::stringstream ss;
        for(const auto & b : _data)
            ss << std::bitset<CHAR_BIT>(b) << '\'';
        return ss.str();
    }

    // ==================== data access =================================

    [[maybe_unused]] uint8_t* data() noexcept { return _data.data(); };

    /** @return size in bits */
    [[nodiscard]] size_t size() const noexcept { return _bitCounter; };

    // ==================== static utils ================================

    /**
     * @param value source
     * @param start starting bit
     * @param quantity number of bits
     * @return number composed from value[start, start+value] bits
     */
    template<typename T>
    static T getBits(T value, size_t start, size_t quantity) noexcept
    {
        unsigned long long mask = (1u << quantity) - 1u;
        return (value & (mask << start)) >> start;
    }

    template<typename T>
    static size_t _rightmostSetBit(T value) noexcept
    {
        return log2(value & -value);
    }

    template<typename T>
    static size_t _leftmostSetBit(T value) noexcept
    {
        size_t length = sizeof(T) - 1;

        while (!getByte(value, length) && length )
            --length;

        return _leftmostSetBit(getByte(value, length)) + length * CHAR_BIT;
    }

    [[nodiscard]] static bool isBigEndian() noexcept
    {
        int num = 1;
        return *(char *)&num != 1;
    }

    /**
     * @return n-th byte of the value
     */
    template<typename T>
     static uint8_t getByte(T value, size_t n) noexcept
    {
        return ((uint8_t*)(&value))[n];
    }


    std::vector<uint8_t> _data{}; ///< Actual data
    size_t _bitCounter{}; ///< Last written bit
};

template<>
inline size_t BitBuffer::_leftmostSetBit<uint8_t>(uint8_t value) noexcept
{
    size_t pos = 0;
    while (value >>= 1u)
        pos++;

    return pos;
}