/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief A TCP client implementation for communicating with a Sockum server.
 * 
 * The SockumClient class is a specialized network client that extends the SockumNetworkEntity base class.
 * It handles connection to a server, manages custom routes for message handling, supports file transfer,
 * and provides an easy-to-use interface for sending/receiving structured messages via predefined routes.
 * 
 * Core functionalities include:
 * - Connecting to a remote server over TCP
 * - Registering custom and file-based routes for handling incoming messages
 * - Routing outgoing messages to the server
 * - Receiving and dispatching messages through a dedicated listener
 * - Managing a unique client ID assigned by the server
 */


#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "networkEntity.hpp"
#include "utils/serialize.hpp"

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

        void routeFileProcess(map<string,any>& args, const string &save_path);

        bool clientReady = false;

    public:
        SockumClient();
        SockumClient(int PORT);
        ~SockumClient();

        string getClientID(){ return client_id; }

        void run();

        SockumClient* addRoute(string route, function<void(map<string, any>)> funcRoute);
        SockumClient* addFileRoute(string route,const string path = "");

        template <typename T>
        SockumClient* addRoute(string route, function<void(T, map<string, any>)> funcRoute);

        SockumClient* route(string route, map<string, any>& args);
        SockumClient* routeFile(string route, string path);

        void disconnect() { close(serverSocket); };

        bool createRoom(std::string room_name,  int unique_id = -1);
        bool join(int room_id, std::string room_name = "");
        bool leave(int room_id,  std::string room_name = "");
};

#endif