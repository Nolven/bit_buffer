#include "BinaryMessage.h"
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    BinaryMessage msg(100);

    msg.Append(3, 2);
    //::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
}
