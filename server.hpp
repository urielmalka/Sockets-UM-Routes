#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

using namespace std;

class Server
{
    private:
        
        void initServer();
        int serverSocket;
        int server_port;
        int max_connection;


    public:
        Server();
        Server(int PORT);
        Server(int PORT, int max_conn);
        ~Server();

        void addRoute();
    
};




#endif