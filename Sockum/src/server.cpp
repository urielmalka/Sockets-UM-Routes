#include "server.hpp"

SockumServer::SockumServer()
{
    server_port = 8080;
    max_connection = 128;
    initServer();
    coreRoutes();
}

SockumServer::SockumServer(int PORT)
{
    if(PORT < 1) throw runtime_error("PORT must be greater than zero.");
    server_port = PORT;
    max_connection = 128;
    initServer();
}

SockumServer::SockumServer(int PORT, int max_conn)
{
    if(max_conn < 1 || PORT < 1) throw runtime_error("Values must be greater than zero.");

    server_port = PORT;
    max_connection = max_conn;
    initServer();
}

SockumServer::~SockumServer()
{
    for(auto &t : tasks)
    {
        t.join();
    }
    close(serverSocket);
}

void SockumServer::initServer()
{
    serverSocket =  socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if( bind(serverSocket, (struct sockaddr*)&serverAddress,sizeof(serverAddress)) < 0)
    {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // listening to the assigned socket
    if(listen(serverSocket, max_connection) < 0)
    {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);  
    }
}

void SockumServer::run()
{
    cout << "Server is running on PORT: " << server_port << endl;
    
    int new_socket;

    while (1){
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        if ((new_socket = accept(serverSocket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept");
            continue;
        }

        string tempId;
        tempId = generateClientId();
        
        while (clients.count(tempId) > 0)
        {
            tempId = generateClientId();
        }
        
        clients[tempId] = new_socket;

        mangePack->add_cid(tempId);

        map<string, any> response;

        response["cid"] = tempId;
        route("/setId", response,new_socket);

        thread thread_client([this, new_socket](){
            listenerRoutes(new_socket);
        });
        thread_client.detach();
        

    }
}

void SockumServer::listenerRoutes(int client_id)
{   

    string route;
    map<string, any> pack;
    int message_id;

    printcb(GREEN, "Client %d is connect now.\n", client_id);

    while (1)
    {
        char buffer[MAX_CHUNK_SIZE]= {0};
        size_t actual_size = MAX_CHUNK_SIZE;

        if (!recv_all(client_id,buffer, actual_size)) break;

        string strBuffer(buffer,actual_size);

        // need to optimaze
        if(!mangePack->manegePack(getClientBySocketID(client_id),strBuffer,message_id)) continue;

        string packBuffer = mangePack->getPack(getClientBySocketID(client_id),message_id);
        
        string t = "~";
        vector<string> result = splitByDelimiter(packBuffer.c_str(), t);

        for (const string& s : result) {
            
            char* c = new char[s.size() + 1];

            strcpy(c, s.c_str());

            int index = serialize_route(c, &route);
            if(index == ERROR_ROUTE){
                printcu(RED,"Error route socket: %s\n",s.substr(0,50).c_str());
                continue;
            };

            memmove(c, c + index, s.size() + 1);
            if(serialize_str(c, &pack) == ERROR_SERIALIZE_PACK){
                printcu(RED,"Error pack socket: %s\n",s.substr(0,50).c_str());
                continue;
            };

            pack["cid"] = getClientBySocketID(client_id);
            logGet(pack);
            try{
                auto func = routes.at(route);
                func(pack);
            }catch(const out_of_range &e){
                printc(RED,"Error: %s not found\n",route.substr(0,50).c_str());
            }

            delete[] c;
            pack.clear();
        }

        
        

    }
    string cid = getClientBySocketID(client_id);
    printcb(RED, "Client %s is disconnect now.\n", cid.c_str());
    clients.erase(cid);
    mangePack->remove_cid(cid);
    close(client_id);
    
}

SockumServer* SockumServer::addRoute(string route, function<void(map<string, any>&)> funcRoute)
{
    routes[route] = funcRoute;
    return this;
}

template <typename T>
SockumServer* SockumServer::addRoute(string route, function<void(T, map<string, any>& )> funcRoute)
{
    serverRoutes[route] = funcRoute;
    return this;
}

SockumServer* SockumServer::route(string route, map<string, any>& args, int client_id)
{
    string buffer = route + AT_SIGN_ROUTE + serialize_map(args);

    logSend(args);
    int msgID = generateMessageID();
    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);
    for(const string &p : packs){
        uint32_t len = htonl(p.size());
        send(client_id, &len, sizeof(len), 0);
        send(client_id, p.c_str(), p.size(), MSG_NOSIGNAL);
    }
    
    return this;
}


SockumServer* SockumServer::sendMessageToClient(string route ,string cid ,map<string, any> &args) 
{
    string buffer = route + AT_SIGN_ROUTE + serialize_map(args);
    int client_id = clients[cid];
    int msgID = generateMessageID();
    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

    for(const string &p : packs){
        uint32_t len = htonl(p.size());
        send(client_id, &len, sizeof(len), 0);
        send(client_id, p.c_str(), p.size(), 0);
    }

    return this;
};
SockumServer* SockumServer::sendMessageToClientList(string route ,list<string> cids, map<string, any> &args) 
{
    string buffer = route + AT_SIGN_ROUTE + serialize_map(args);
    int msgID = generateMessageID();
    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);
    int client_id;

    for(const string &p : packs){
        uint32_t len = htonl(p.size());
        for(string &cid : cids){
            client_id = clients[cid];
            send(client_id, &len, sizeof(len), 0);
            send(client_id, p.c_str(), p.size(), 0);
        }
    }

    return this;
};

SockumServer* SockumServer::sendMessageToAll(string route ,map<string, any> &args) 
{
    logSend(args);
    
    string buffer = route + AT_SIGN_ROUTE + serialize_map(args);
    int msgID = generateMessageID();
    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

    for(auto &client : clients){
        for(const string &p : packs){
            uint32_t len = htonl(p.size());
            send(client.second, &len, sizeof(len), 0);
            ssize_t bytes_sent = send(client.second, p.c_str(), p.size(), MSG_NOSIGNAL);
            if(bytes_sent < 0)
                printc(RED, "Send Faild\n");
        }
    }

    return this;
};
SockumServer* SockumServer::closeClientConnection(string cid) 
{
    int client_id = clients[cid];
    close(client_id);

    return this;
};


void SockumServer::coreRoutes()
{   
    //addRoute("/setId", [this](map<string, any> args) {setClientId(args); } );

}

string SockumServer::getClientBySocketID(int sid)
{
    for (auto it = clients.begin(); it != clients.end(); ++it)
    {
        if(it->second == sid) return it->first;
    }

    return CLIENT_NOT_FOUND;
}


void SockumServer::logGet(map<string, any> &args)
{
    if(!logActivied) return;

    printcu(YELLOW, "SERVER GET:");
    printc(YELLOW, "\n\t{\n\t\t");
    for(auto &pair : args)
    {
        printc(YELLOW, "\"%s\": \"%s\" \n\t\t",pair.first.substr(0,50).c_str(), any_cast<string>(pair.second).substr(0,50).c_str());
    }
    printc(YELLOW, "\b\b\b\b\b\b\b\b}\n");
}

void SockumServer::logSend(map<string, any> &args)
{
    if(!logActivied) return;

    printcu(GREEN, "SERVER SEND:");
    printc(GREEN, "\n\t{\n\t\t");
    for(auto &pair : args)
    {
        printc(GREEN, "\"%s\": \"%s\" \n\t\t",pair.first.substr(0,50).c_str(), any_cast<string>(pair.second).substr(0,50).c_str());
    }
    printc(GREEN, "\b\b\b\b\b\b\b\b}\n");
}


/* This function generate id for the Class Client */
string generateClientId()
{
    string cid;
    for (int i = 0; i < 8; ++i) {cid += chars[rand() % chars.length()];}
    return cid;
}

