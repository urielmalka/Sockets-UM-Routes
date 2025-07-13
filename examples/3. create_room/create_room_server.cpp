#include <Sockum.hpp>


int main()
{
    SockumServer * server = new SockumServer();
    server->setLogActivated(true); // Enable logging for debugging

    server->run();
    
    return 0;
}