#include <Sockum/Sockum.hpp>



void sendMessage(SockumServer *server, map<string, any> & args)
{ 
    server->sendMessageToAll("/sendMessageToAll", args);
}


int main()
{
    SockumServer * server = new SockumServer();


    server->addRoute("/sendMessageToAll", [server](map<string, any> args) {
        sendMessage(server, args);
    });


    server->run();
    
    return 0;
}