#include "server.hpp"
#include "client.hpp"



void sendMessage(map<string, any> args)
{ 
    cout << "SendMessgae" << endl;
    for (const auto& pair : args)
    {
        std::cout << pair.first << endl;
    }
}


int main()
{
    /*
    Server * server = new Server();


    server->addRoute("/sendMessage",sendMessage);

    server->run();
    */

    Client *c = new Client();
    c->run();
    
    return 0;
}