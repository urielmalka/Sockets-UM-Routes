#include <Sockum/Sockum.hpp>



void sendMessage(Server *server, map<string, any> & args)
{ 
    server->sendMessageToAll("/sendMessageToAll", args);
}


int main()
{
    Server * server = new Server();


    server->addRoute("/sendMessageToAll", [server](map<string, any> args) {
        sendMessage(server, args);
    });


    server->run();
    
    return 0;
}