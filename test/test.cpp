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
    std::uniform_int_distribution<uint64_t> vdist(0, std::numeric_limits<uint64_t>::max());

    while (length)
    {
        uint64_t number = vdist(gen);
        std::uniform_int_distribution<size_t> ldist(64, length);
        size_t l = ldist(gen);
        length -= l;
        out.emplace_back(number, l);
    }

    return out;
}

TEST(meh, hmeh)
{
    auto m = generateValueLengthPairs(800);
    BinaryMessage msg(800);

    for(auto & item : m)
    {
        msg.Append(item.first, item.second);
    }

    size_t counter = 0;
    for(auto & item : m)
    {
        ASSERT_EQ(msg.Get<uint64_t>(counter, item.second), item.first);
        counter += item.second;
    }

}
