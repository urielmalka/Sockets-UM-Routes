#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <color.h>
#include <map>
#include <any>
#include <string.h>
#include <functional>
#include <iostream>

#include "serialize.hpp"

#define BUFFER_SIZE 1024  

using namespace std;

class Client
{
    private:

        string client_id;

        int serverSocket;
        int server_port;  
        sockaddr_in serverAddress;

        map<string, function<void(map<string, any>)>> routes;

        void initClient();
        void connectClient();
        void coreRoutes();
        void setClientId(map<string,any>& args);
        
        void listenerRoutes();


    public:
        Client();
        Client(int PORT);
        ~Client();

        void run();
        void addRoute(string route, function<void(map<string, any>)> funcRoute);
        void route(string route, map<string, any>& args);
};


#endif