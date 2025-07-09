#include <Sockum.hpp>


int main()
{
    SockumServer* server = new SockumServer();


    server->addRoute("file", ([server](map<string, any> args) {
        server->sendMessageToAll("file", args);
    }));

    server->run();

    return 0;
}
