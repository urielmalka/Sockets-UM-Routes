#include <Sockum.hpp>

#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <atomic>

int main()
{

    SockumServer *server = new SockumServer();

    server->addRoute("broadcast", [server](std::map<std::string, std::any> args) {
        std::string from = any_cast<std::string>(args["from"]);
        std::string message = any_cast<std::string>(args["message"]);
        std::cout << "[" << from << "] " << message << std::endl;
        server->sendMessageToAll(
            "broadcast", args
        );
    });

    server->run();

    delete server;

    return 0;
}


