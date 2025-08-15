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

    server->addRoute("sendGroup", [server](std::map<std::string, std::any> args) {
        std::string username = any_cast<std::string>(args["username"]);
        std::string message = any_cast<std::string>(args["message"]);
        std::cout << "[" << username << "]: " << message << std::endl;
        server->sendMessageToAll(
            "sendGroup", args
        );
    });

    server->run();

    delete server;

    return 0;
}