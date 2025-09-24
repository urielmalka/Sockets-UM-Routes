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

    c1->addRoute("broadcast", [](std::map<std::string, std::any> args){

        std::string from = any_cast<std::string>(args["from"]);
        std::string message = any_cast<std::string>(args["message"]);

        std::cout << "[" << from << "] " << message << std::endl;
    });
    
    std::thread t([c1]() { c1->run(); });
    t.detach();

    std::string username;
    std::cout << "Pick username:";
    std::getline(std::cin, username);

    std::string input;
    std::map<std::string, std::any> args;


    while (true) {
        std::getline(std::cin, input);
        if (input == "Q" || input == "q") {
            break;
        }

        args["from"] = username;
        args["message"] = input;
        c1->route("broadcast", args);
    }

    std::cout << "Exiting...\n";
    c1->disconnect();
    delete c1;
    return 0;
}


