#include <Sockum.hpp>


int main()
{
    SockumServer * server = new SockumServer();

    server->run();
    
    return 0;
}