#ifndef RBC_GW_BINARYMESSAGE_H
#define RBC_GW_BINARYMESSAGE_H

#include <iostream>
#include <vector>
#include <bitset>

class BinaryMessage
{
public:
    explicit BinaryMessage(size_t length);
    ~BinaryMessage() = default;

    /**
     * //TODO define
     * @tparam T Value type
     * @param value Value to write
     * @param start Where to start writing
     * @param length Length to write
     * @param bigEndian order
     * @return bits written
     */
    template<typename T>
    size_t Insert(T value, size_t start, size_t length, bool bigEndian = true) = delete;

    /**
     * Writes value to the end of binary message
     * @param value Value to add
     * @param length In bits
     * @return number Added length
     */
    template<typename T>
    size_t Append(T value, size_t length, bool bigEndian = true)
    {
        static_assert(!std::is_same<std::string, T>::value, "String are not allowed!");

        size_t ret = length;

        size_t byte = _bitCounter / 8;
        size_t bitLeftInByte = 8 - _bitCounter % 8;

        size_t valueStartBit = 0;

        while( length > bitLeftInByte )
        {
            _data[byte] |= _getBits(value, bigEndian ? length - bitLeftInByte : valueStartBit, bitLeftInByte);

            valueStartBit += bitLeftInByte;
            _bitCounter += bitLeftInByte;
            length -= bitLeftInByte;

            bitLeftInByte = 8 - _bitCounter % 8;
            byte = _bitCounter / 8;
        }

        _data[byte] |= _getBits(value, bigEndian ? bitLeftInByte - length : valueStartBit, length) << (bitLeftInByte - length);
        _bitCounter += length;

        return ret;
    }

    /**
     * @return uint_8t data
     */
    [[maybe_unused]] uint8_t* Data() noexcept;
    [[maybe_unused]] char* DataInChar();

    /**
     * @return size Current size in bits
     */
    [[maybe_unused]] [[nodiscard]] size_t Size() const noexcept;

private:
    /**
     * Get bits from value without shift (i.e. taken bits would be first)
     * @tparam T type
     * @param value Source
     * @param start start bit
     * @param quantity number of bits
     * @return
     */
    template<typename T>
    inline static uint8_t _getBits(T value, size_t start, size_t quantity) noexcept
    {
        return (value & (((1u << quantity) - 1u) << start)) >> start;
    }

public:
    /**
     * Returns most significant bit index
     * @tparam T input type
     * @param val value to find in
     * @return index of most significant set bit
     */
    template<typename T>
    inline static size_t _mostSigSetBit(T val)
    {
        size_t index;
        if( _littleEndian() )
            index = _leftmostSetBit(val);
        else
            index = _rightmostSetBit(val);

        return index;
    }

    /**
     * Finds index of leftmost set bit
     * @tparam T input type
     * @param value Where to search
     * @return index (from left (for example for 00(1)00101 it would be 2)
     */
    template<typename T>
    inline static size_t _leftmostSetBit(T value) noexcept
    {
        size_t length = sizeof(T);

        while ( !_getByte(value, length--) && length ){}

        return sizeof(T) - length;
    }

    /**
     * Finds index of leftmost set bit
     * @tparam T input type
     * @param value Where to search
     * @return index (from left (for example for 00(1)00101 it would be 2)
     */
    template<typename T>
    inline static size_t _rightmostSetBit(T value) noexcept
    {
        size_t length = sizeof(T);

        while ( !_getByte(value, length--) && length ){}

        return sizeof(T) - length;
    }

public:
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
     * @param value value to get byte from
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

    std::vector<uint8_t> _data; ///< Actual data //TODO don't actually need vector, can be changed to uint8_t*
    size_t _bitCounter; ///< Last written bit
};

#endif //RBC_GW_BINARYMESSAGE_H