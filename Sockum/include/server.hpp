/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief A TCP server implementation for managing multiple Sockum clients.
 * 
 * The SockumServer class is a specialized network server that extends the SockumNetworkEntity base class.
 * It manages incoming client connections, provides routing of messages based on route identifiers,
 * supports sending messages to individual clients, groups of clients, or all connected clients,
 * and enables server-side route logic to handle requests from clients.
 * 
 * Core functionalities include:
 * - Listening for and handling multiple client connections via threads
 * - Routing incoming messages to custom logic via registered route functions
 * - Tracking connected clients via unique client IDs
 * - Broadcasting or directing messages with structured data (`map<string, any>`)
 * - Supporting core internal routes such as client registration and server logging
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "networkEntity.hpp"
#include "utils/serialize.hpp"

using namespace std; 

const string chars = "abcdefghijklmnopqrstuvwxyz";
const string CLIENT_NOT_FOUND = "non-cid";
string generateClientId();

class SockumServer : public SockumNetworkEntity
{
    private:
        
        void initServer();

        int max_connection;

        bool logActivied = true;

        map<string, function<void(map<string, any>&)> > routes;
        map<string, function<void(SockumServer, map<string, any>&)> > serverRoutes;
        map<string, int> clients;

        vector<thread> tasks;

        void listenerRoutes(int client_id);

        /* Core Functions Routes */
        void coreRoutes();
        /*End of Core Functions Routes */

        string getClientBySocketID(int sid);

        void logGet(map<string, any> &args);
        void logSend(map<string, any> &args);

    public:
        SockumServer();
        SockumServer(int PORT);
        SockumServer(int PORT, int max_conn);
        ~SockumServer();



        SockumServer* sendMessageToClient(string route ,string cid ,map<string, any> &args);
        SockumServer* sendMessageToClientList(string route ,list<string> cids, map<string, any> &args);
        SockumServer* sendMessageToAll(string route ,map<string, any> &args, bool include_sender = false );
        SockumServer* closeClientConnection(string cid);
         

        void run();
        SockumServer* addRoute(string route, function<void(map<string, any>&)> funcRoute);

        template <typename T>
        SockumServer* addRoute(string route, function<void(T, map<string, any>&)> funcRoute);

        SockumServer* route(string route, map<string, any>& args, int client_id);
    
};


#endif