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

    std::map<std::string, std::any> args;
    args["message"] = "Hello from client 1!";

    c1->route("hello_client", args);

    std::cout << "Exiting...\n";
    c1->disconnect();
    delete c1;
    return 0;
}
