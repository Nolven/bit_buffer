#include <gtest/gtest.h>

#include "BinaryMessage.h"

#include <map>
#include <random>
#include <chrono>

template<typename T>
std::vector<std::pair<size_t, T>> generateValueLengthPairs(size_t length)
{
    std::vector<std::pair<size_t, T>> out;

    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
            (std::mt19937::result_type)
                    std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch()
                    ).count() +
            (std::mt19937::result_type)
                    std::chrono::duration_cast<std::chrono::microseconds>(
                            std::chrono::high_resolution_clock::now().time_since_epoch()
                    ).count() );

    std::mt19937 gen(seed);

    while (length)
    {
        std::uniform_int_distribution<size_t> ldist(0, length);
        size_t l = ldist(gen) % (sizeof(T) * 8);

        // If `l` > 64, then we can generate any number of given type. Otherwise `l` would be the leftmost set bit
        // for upper bound. In other words we can generate any number [0, 2^l-1]
        T upperLimit = static_cast<T>(std::pow(2, l) - 1);
        std::uniform_int_distribution<T> vdist(0, upperLimit);

        T number = vdist(gen);
        length -= l;

        std::cout << "Pushed: " << number << " ; " << l << '\n';
        out.emplace_back(number, l);
    }

    return out;
}

TEST(Insert, integral)
{
    BinaryMessage msg(100);

    auto m = generateValueLengthPairs<uint64_t>(100);
    for(auto & item : m)
    {
        msg.Append(item.first, item.second);
    }

    std::cout << msg << '\n';

    size_t counter = 0;
    for(auto & item : m)
    {
        std::cout << "=========================TEST CASE=====================================\n";

        std::cout << "Counter: " << counter << "; Length: " << item.second << '\n';
        std::cout << "EXP: " << std::bitset<64>(item.first) << '\n';

        auto result = msg.Get<uint64_t>(counter, item.second);
        std::cout << "GOT: " << std::bitset<64>(result) << '\n';

        if( result != item.first )
        {
            std::cout << "Pit stop\n";
            result = msg.Get<uint64_t>(counter, item.second);
        }

        EXPECT_EQ(result, item.first);

        std::cout << "=========================TEST CASE=====================================\n\n";

    }

}
