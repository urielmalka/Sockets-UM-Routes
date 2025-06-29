#include <Sockum.hpp>

#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <atomic>

int main()
{
    SockumClient *c1 = new SockumClient();

    std::thread t([c1]() { c1->run(); });
    t.detach();

    c1->createRoom("Test Room", 1500);

    std::string input;

    while (true) {
        std::getline(std::cin, input);
        if (input == "Q" || input == "q") {
            break;
        }
    }

    std::cout << "Exiting...\n";
    c1->disconnect();
    delete c1;
    return 0;
}
