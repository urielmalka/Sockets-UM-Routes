#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "Sockum/networkEntity.hpp"
#include "Sockum/utils/serialize.hpp"

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


SockumClient::SockumClient()
{
    server_port = 8080;
    initClient();
}

SockumClient::SockumClient(int PORT)
{
    if(PORT < 1) throw runtime_error("PORT must be greater than zero.");
    server_port = PORT;
    initClient();
}

SockumClient::~SockumClient()
{
    close(serverSocket);
}

void SockumClient::initClient()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connectClient();
    coreRoutes();
    
}

void SockumClient::connectClient()
{
    int count_try_connection = 0, is_connected;
    while (count_try_connection < 5)
    {
        is_connected = connect(serverSocket, (struct sockaddr*)&serverAddress,sizeof(serverAddress));
        if(is_connected == 0 ) {
            mangePack->add_cid("local");
            printc(GREEN,"Client connect to port %d.\n",server_port);
            break;
        };
        printc(RED,"(%d) : Client failed to connect to port %d\n",count_try_connection,server_port);
        count_try_connection ++;
        this_thread::sleep_for(chrono::seconds(5));

    }
    
}

void SockumClient::listenerRoutes()
{   

    string route;
    map<string, any> pack;

    while (1)
    {
        char buffer[BUFFER_SIZE] = {0};

        int bytes_received = recv(serverSocket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) break;

    
        /*if(isCryptp){
            string de_buffer = decrypt(buffer);
            strncpy(buffer,de_buffer.c_str(),BUFFER_SIZE - 1);
        }*/

        string strBuffer(buffer,bytes_received);

        if(!mangePack->manegePack("local",strBuffer)) continue;

        string packBuffer = mangePack->getPack("local");

        string t = "~";
        vector<string> result = splitByDelimiter(packBuffer, t);

        for (const string& s : result) {
            
            char* c = new char[s.size() + 1];

            strcpy(c, s.c_str());

            int index = serialize_route(c, &route);
            if(index == ERROR_ROUTE){
                printcu(RED,"Error route socket: %s\n",s.c_str());
                continue;
            };

            memmove(c, c + index, s.size() + 1);
            if(serialize_str(c, &pack) == ERROR_SERIALIZE_PACK){
                printcu(RED,"Error pack socket: %s\n",s.c_str());
                continue;
            };
  
            try{
                auto func = routes.at(route);
                func(pack);
            }catch(const out_of_range &e){
                printc(RED,"Error: %s not found\n",route.c_str());
            }

            delete[] c;
            pack.clear();
        }
    }

    printcb(RED, "Client %s is disconnect now\n",client_id.c_str());
    
    close(serverSocket);
    
}

SockumClient* SockumClient::addRoute(string route, function<void(map<string, any>)> funcRoute)
{
    routes[route] = funcRoute;
    return this;
}

template <typename T>
SockumClient* SockumClient::addRoute(string route, function<void(T, map<string, any>)> funcRoute)
{
    routes[route] = funcRoute;
    return this;
}

SockumClient* SockumClient::route(string route, map<string, any>& args)
{
    string buffer = route + "@" + serialize_map(args);
    vector<string> packs = mangePack->chunk_string_for_network(buffer);

    for(const string &p : packs){
        send(serverSocket, p.c_str(), p.size(), 0);
    }



    return this;
}

void SockumClient::run(){
    listenerRoutes();
}


void SockumClient::setClientId(map<string, any>& args)
{
    client_id = any_cast<string>(args["cid"]);
    printcb(GREEN, "Client %s is connect now.\n",client_id.c_str());
}

void SockumClient::coreRoutes()
{   
    addRoute("/setId", [this](map<string, any> args) {setClientId(args); } );
}

#endif