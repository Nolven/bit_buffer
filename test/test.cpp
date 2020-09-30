#include <gtest/gtest.h>

#include "BinaryMessage.h"

#include <map>
#include <random>
#include <chrono>

std::vector<std::pair<size_t, uint64_t>> generateValueLengthPairs(size_t length)
{
    std::vector<std::pair<size_t, uint64_t>> out;

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
        size_t l = ldist(gen);

        // If `l` > 64, then we can generate any number of given type. Otherwise `l` would be the leftmost set bit
        // for upper bound. In other words we can generate any number [0, 2^l-1]
        size_t upperLimit = (l > 64) ? std::numeric_limits<uint64_t>::max() : static_cast<size_t>(std::pow(2, l) - 1);
        std::uniform_int_distribution<uint64_t> vdist(0, upperLimit);

        uint64_t number = vdist(gen);
        length -= l;

        out.emplace_back(number, l);
    }

    return out;
}

TEST(Insert, integral)
{
    auto m = generateValueLengthPairs(800);
    BinaryMessage msg(800);

    for(auto & item : m)
    {
        msg.Append(item.first, item.second);
    }

    std::cout << msg << '\n';

    size_t counter = 0;
    for(auto & item : m)
    {
        ASSERT_EQ(msg.Get<uint64_t>(counter, item.second), item.first);
        counter += item.second;
    }

}
