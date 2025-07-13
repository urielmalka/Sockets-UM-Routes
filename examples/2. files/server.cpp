#include <Sockum.hpp>


int main()
{
    SockumServer* server = new SockumServer();

    server->setLogActivated(true); // Enable logging for debugging

    server->addRoute("file", ([server](map<string, any> args) {
        server->sendMessageToAll("file", args);
    }));

    server->run();

    return 0;
}
