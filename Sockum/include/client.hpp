#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "networkEntity.hpp"
#include "utils/serialize.hpp"

#define BUFFER_SIZE 1024  

using namespace std;

class SockumClient : public SockumNetworkEntity
{
    private:
        string client_id;

        sockaddr_in serverAddress;

        map<string, function<void(map<string, any>)>> routes;

        void initClient();
        void connectClient();
        void coreRoutes();
        void setClientId(map<string,any>& args);
        
        void listenerRoutes();

    public:
        SockumClient();
        SockumClient(int PORT);
        ~SockumClient();

        string getClientID(){ return client_id; }

        void run();
        SockumClient* addRoute(string route, function<void(map<string, any>)> funcRoute);

        template <typename T>
        SockumClient* addRoute(string route, function<void(T, map<string, any>)> funcRoute);

        SockumClient* route(string route, map<string, any>& args);

        void disconnect() { close(serverSocket); };
};

#endif