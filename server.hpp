#ifndef SERVER_HPP
#define SERVER_HPP

#include "networkEntity.hpp"
#include "serialize.hpp"

using namespace std;

#define BUFFER_SIZE 1024  

const string chars = "abcdefghijklmnopqrstuvwxyz";
const string CLIENT_NOT_FOUND = "non-cid";
string generateClientId();

class Server : public NetworkEntity
{
    private:
        
        void initServer();
        int serverSocket;
        int server_port;
        int max_connection;

        bool logActivied = true;

        map<string, function<void(map<string, any>&)> > routes;
        map<string, function<void(Server, map<string, any>&)> > serverRoutes;
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
        Server();
        Server(int PORT);
        Server(int PORT, int max_conn);
        ~Server();



         void sendMessageToClient(string route ,string cid ,map<string, any> &args);
         void sendMessageToClientList(string route ,list<string> cids, map<string, any> &args);
         void sendMessageToAll(string route ,map<string, any> &args);
         void closeClientConnection(string cid);
         

        void run();
        void addRoute(string route, function<void(map<string, any>&)> funcRoute);
        void addRoute(string route, function<void(Server, map<string, any>&)> funcRoute);
        void route(string route, map<string, any>& args, int client_id);
    
};




#endif