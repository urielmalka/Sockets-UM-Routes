#include <Sockum.hpp>

#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <atomic>


int main()
{
    SockumClient *c2 = new SockumClient();

    thread t([c2]() { c2->run(); });
    t.detach();
    
    c2->join(1500);

    std::string input;
    
    while (true) {
        std::getline(std::cin, input);
        if (input == "Q" || input == "q") {
            break;
        }
    }

    std::cout << "Exiting...\n";

    c2->disconnect();
    delete c2;
    
    return 0;
}