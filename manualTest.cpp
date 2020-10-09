#include "BinaryMessage.h"

int main() {
    std::cout << "EMPTY\n";

    BinaryMessage msg(40); //less then

    msg.Append(1, 2);
    msg.Append(3, 2);
    msg.Append(5, 4);
    msg.Append(3, 2);
    msg.Append(5, 2);
    msg.Append(3, 2);
    msg.Append(5, 2);
    msg.Append(3, 2);
    msg.Append(5, 2);
    msg.Append(3, 2);
    msg.Append(5, 2);

    size_t start = 0;
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 4) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';std::cout << msg.Get<int>(start, 2) << '\n';
    std::cout << msg.Get<int>(start, 2) << '\n';

}