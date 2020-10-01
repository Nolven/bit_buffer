#ifndef RBC_GW_BINARYMESSAGE_H
#define RBC_GW_BINARYMESSAGE_H

#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>

class BinaryMessage
{
public:
    explicit BinaryMessage(size_t length);
    ~BinaryMessage() = default;

    /**
     * @tparam T Value type
     * @param value Value to write
     * @param writeStart Where to start writing
     * @param length Length to write
     * @param bigEndian order
     * @return bits written
     */
    template<typename T>
    size_t Insert(T value, size_t writeStart, size_t length)
    {
        size_t leftMost = _leftmostSetBit(value);

        // If passed value requires more bits then passed
        if( leftMost > length )
            std::cout << "Warning, length is too small\n";

        size_t valueStart = length;

                //TODO debug
                std::cout << "Value: " << std::bitset<sizeof(T)*8>(value) << '\n';
                std::cout << "Length: " << length << '\n';

        size_t byte = writeStart / 8; ///< Byte in _data to write to
        size_t bitsLeftInByte = 8 - writeStart % 8; ///< bits left in current byte
        size_t bitsLeftToWrite = length; ///< remaining length

        while (bitsLeftToWrite > bitsLeftInByte)
        {
            valueStart -= bitsLeftInByte; // We count from right to left (cause bits)
            _data[byte] |= _getBits(value, valueStart, bitsLeftInByte);

            writeStart += bitsLeftInByte;
            bitsLeftToWrite -= bitsLeftInByte;

            ++byte;
            bitsLeftInByte = 8;
        }

        _data[byte] |= _getBits(value, 0, bitsLeftToWrite) << (8 - bitsLeftToWrite);

                //TODO debug
                std::cout << *this << '\n';
                std::cout << "========\n";

        return length;
    }

    /**
     * Writes value to the end of binary message
     * @param value Value to add
     * @param length In bits
     * @return number Added length
     */
    template<typename T>
    size_t Append(T value, size_t length)
    {
        size_t pos = Insert(value, _bitCounter, length);
        _bitCounter += length;
        return pos;
    }

    /**
     * @return uint_8t data
     */
    [[maybe_unused]] uint8_t* Data() noexcept;
    [[maybe_unused]] char* DataInChar();

    /**
     * Get's value from array
     * @tparam T return type
     * @param start position of the first bit
     * @param length length of value
     * @return value
     */
    template<typename T>
    T Get(const size_t& start, size_t length)
    {
        size_t buff = start;
        return Get<T>(buff, length);
    }

    /**
     * Get's value from array
     * @tparam T return type
     * @param start position of the first bit
     * @param length length of value
     * @return value
     */
    template<typename T>
    T Get(size_t& start, size_t length)
    {
        T number{};

        //Todo debug
        std::cout << "Start: " << start;
        std::cout << "; Length: " << length << '\n';

        size_t bitsToTake = length;
        size_t byte = start / 8;

        uint8_t bitsLeftInByte = 8u - (start % 8u);

        while (bitsToTake > bitsLeftInByte)
        {
            number |= _getBits(_data[byte], 0, bitsLeftInByte);

            start += bitsLeftInByte;
            bitsToTake -= bitsLeftInByte;

            bitsLeftInByte = 8;
            ++byte;

            number <<= bitsToTake;
        }

        number |= _getBits(_data[byte], 8 - bitsToTake, bitsToTake);

        return number;
    }

    /**
     * @return size Current size in bits
     */
    [[maybe_unused]] [[nodiscard]] size_t Size() const noexcept;

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
        T mask = (1u << quantity) - 1u;
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
        {
            --length;
        }

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

    /**
     * Swaps endiannes
     * @tparam T Input type
     * @param u value
     * @return swapped number
     */
    template <typename T>
    T _endSwap(T u)
    {
        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof(T); k++)
            dest.u8[k] = source.u8[sizeof(T) - k - 1];

        return dest.u;
    }

    friend std::ostream& operator<<(std::ostream& os, const BinaryMessage& dt);

    std::vector<uint8_t> _data; ///< Actual data //TODO don't actually need vector, can be changed to uint8_t*
    size_t _bitCounter; ///< Last written bit
};

template<>
inline size_t BinaryMessage::_leftmostSetBit<uint8_t>(uint8_t value) noexcept
{
    size_t pos = 0;
    while (value >>= 1u)
        pos++;

    return pos;
}

#endif //RBC_GW_BINARYMESSAGE_H