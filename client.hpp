#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "networkEntity.hpp"
#include "serialize.hpp"

#define BUFFER_SIZE 1024  

using namespace std;

class Client : public NetworkEntity
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
        
        void cryptoHandler(function<string(string)> encryptionFunction,function<string(string)> decryptionFunction);


    public:
        Client();
        Client(int PORT);
        ~Client();

        string getClientID(){ return client_id; }

        void run();
        void addRoute(string route, function<void(map<string, any>)> funcRoute);
        void route(string route, map<string, any>& args);
};


#endif