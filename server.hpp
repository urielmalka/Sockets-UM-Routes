#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <map>
#include <any>
#include <string>
#include <functional>
#include <thread>
#include <color.h>
#include <ctime>

#include "serialize.hpp"

using namespace std;

#define BUFFER_SIZE 1024  

const string chars = "abcdefghijklmnopqrstuvwxyz";
string generateClientId();

class Server
{
    private:
        
        void initServer();
        int serverSocket;
        int server_port;
        int max_connection;

        map<string, function<void(map<string, any>)> > routes;
        map<string, int> clients;

        void listenerRoutes(int client_id);


        /* Core Functions Routes */
        void coreRoutes();
        void setClientId(map<string, any> & args);


    public:
        Server();
        Server(int PORT);
        Server(int PORT, int max_conn);
        ~Server();


        void run();
        void addRoute(string route, function<void(map<string, any>)> funcRoute);
        void route(string route, map<string, any>& args, int client_id);
    
};




#endif