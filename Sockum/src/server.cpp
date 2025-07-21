#include "server.hpp"

SockumServer::SockumServer(bool baseLogActivated)
{
    server_port = 8080;
    max_connection = 128;

    setBaseLogActivated(baseLogActivated);
    
    initServer();
    coreRoutes();
    
}

SockumServer::SockumServer(int PORT)
{
    if(PORT < 1) throw runtime_error("PORT must be greater than zero.");
    server_port = PORT;
    max_connection = 128;
    initServer();
    coreRoutes();
}

SockumServer::SockumServer(int PORT, int max_conn)
{
    if(max_conn < 1 || PORT < 1) throw runtime_error("Values must be greater than zero.");

    server_port = PORT;
    max_connection = max_conn;
    initServer();
    coreRoutes();
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

    if( ::bind(serverSocket, (struct sockaddr*)&serverAddress,sizeof(serverAddress)) < 0)
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
    if (baseLogActivated) cout << "Server is running on PORT: " << server_port << endl;

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

        X3DHKeys* x3dh_keys = new X3DHKeys();
        x3dh_keys->generate();

        mangePack->add_crypto_pack(tempId);
        X3DHKeys& server_keys = mangePack->get_crypto_pack(tempId).get_x3dh_keys();

        send(new_socket, server_keys.identity_pub.data(), crypto_kx_PUBLICKEYBYTES, 0);
        send(new_socket, server_keys.signed_prekey_pub.data(), crypto_kx_PUBLICKEYBYTES, 0);
        send(new_socket, server_keys.one_time_pub.data(), crypto_kx_PUBLICKEYBYTES, 0);

        recv(new_socket, x3dh_keys->identity_pub.data(), crypto_kx_PUBLICKEYBYTES, MSG_WAITALL);
        recv(new_socket, x3dh_keys->signed_prekey_pub.data(), crypto_kx_PUBLICKEYBYTES, MSG_WAITALL);
        recv(new_socket, x3dh_keys->one_time_pub.data(), crypto_kx_PUBLICKEYBYTES, MSG_WAITALL);

        mangePack->set_shared_secret(tempId, *x3dh_keys);

        delete x3dh_keys; // Clean up after receiving keys

        clients[tempId] = new_socket;

        mangePack->add_cid(tempId);

        map<string, any> response;

        response["cid"] = tempId;
        route("setId", response,new_socket);

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

    baseLogActivated ? printcb(GREEN, "Client %d is connect now.\n", client_id) : void();

    while (1)
    {
        char buffer[MAX_CHUNK_SIZE * 2]= {0};
        size_t actual_size = MAX_CHUNK_SIZE * 2;

        if (!recv_all(client_id,buffer, actual_size)) break;

        string strBuffer(buffer,actual_size);

        if(logActivated) printc(BLUE, "Received message ID: %d, Buffer: %s\n", message_id, strBuffer.substr(0, 50).c_str());

        auto decrypted = mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_receive_ratchet().decrypt(from_hex(strBuffer));
        SignedMessage signed_message_received = SignedMessage::deserialize(decrypted);

        if(!verify_message(signed_message_received)) {
            if (baseLogActivated) printc(RED, "Received message signature verification failed.\n");
            pack.clear();
            continue;
        }

        if(logActivated) printc(BLUE, "Received message ID: %d, Buffer: %s\n", message_id, signed_message_received.message.substr(0, 50).c_str());

        // need to optimize
        if(!mangePack->manegePack(getClientBySocketID(client_id),signed_message_received.message,message_id)) continue;

        string packBuffer = mangePack->getPack(getClientBySocketID(client_id),message_id);
        
        string t = "~";
        vector<string> result = splitByDelimiter(packBuffer.c_str(), t);

        for (const string& s : result) {
            
            char* c = new char[s.size() + 1];

            strcpy(c, s.c_str());

            int index = serialize_route(c, &route);
            if(index == ERROR_ROUTE){
                if (baseLogActivated) printcu(RED,"Error route socket: %s\n",s.substr(0,50).c_str());
                continue;
            };

            memmove(c, c + index, s.size() + 1);
            if(serialize_str(c, &pack) == ERROR_SERIALIZE_PACK){
                if (baseLogActivated) printcu(RED,"Error pack socket: %s\n",s.substr(0,50).c_str());
                continue;
            };

            pack["cid"] = getClientBySocketID(client_id);
            logGet(pack, route);
            try{
                auto func = routes.at(route);
                func(pack);
            }catch(const out_of_range &e){
                if (baseLogActivated) printc(RED,"Error: %s not found\n",route.substr(0,50).c_str());
            }

            delete[] c;
            pack.clear();
        }

        
        

    }
    string cid = getClientBySocketID(client_id);
    baseLogActivated ? printcb(RED, "Client %s is disconnect now.\n", cid.c_str()) : void();
    clients.erase(cid);
    mangePack->remove_cid(cid);
    close(client_id);
    
}

SockumServer* SockumServer::addRoute(string route, function<void(map<string, any>&)> funcRoute)
{
    if (baseLogActivated) {
        if (routes.find(route) != routes.end()) {
            printci(RED, "Warning: Route already exists. Replacing route: %s\n", route.c_str());
        } else {
            printci(BLUE, "Route Added: %s\n", route.c_str());
        }
    }

    routes[route] = funcRoute;
    return this;
}


template <typename T>
SockumServer* SockumServer::addRoute(string route, function<void(T, map<string, any>& )> funcRoute)
{
    if (baseLogActivated) {
        if (routes.find(route) != routes.end()) {
            printci(RED, "Warning: Route already exists. Replacing route: %s\n", route.c_str());
        } else {
            printci(BLUE, "Route Added: %s\n", route.c_str());
        }
    }
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

        SignedMessage signed_message = sign_message(p, mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_x3dh_keys());
        auto signed_buffer = signed_message.serialize();
        string buffer_encrypted = to_hex(mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_send_ratchet().encrypt(signed_buffer));

        uint32_t len = htonl(buffer_encrypted.size());
        send(client_id, &len, sizeof(len), 0);
        send(client_id, buffer_encrypted.c_str(), buffer_encrypted.size(), MSG_NOSIGNAL);
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
        
        SignedMessage signed_message = sign_message(p, mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_x3dh_keys());
        auto signed_buffer = signed_message.serialize();
        string buffer_encrypted = to_hex(mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_send_ratchet().encrypt(signed_buffer));

        uint32_t len = htonl(buffer_encrypted.size());
        send(client_id, &len, sizeof(len), 0);
        send(client_id, buffer_encrypted.c_str(), buffer_encrypted.size(), 0);
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
        
        SignedMessage signed_message = sign_message(p, mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_x3dh_keys());
        auto signed_buffer = signed_message.serialize();
        string buffer_encrypted = to_hex(mangePack->get_crypto_pack(getClientBySocketID(client_id)).get_send_ratchet().encrypt(signed_buffer));

        uint32_t len = htonl(buffer_encrypted.size());
        for(string &cid : cids){
            client_id = clients[cid];
            send(client_id, &len, sizeof(len), 0);
            send(client_id, buffer_encrypted.c_str(), buffer_encrypted.size(), 0);
        }
    }

    return this;
};

SockumServer* SockumServer::sendMessageToAll(string route ,map<string, any> &args, bool include_sender) 
{
    logSend(args);

    try {
        args.at("cid");
    } catch (const std::out_of_range& e) {
        if (baseLogActivated) printcb(RED,"Missing 'cid' in args: %s\n",e.what());
        return this;
    }

    
    string buffer = route + AT_SIGN_ROUTE + serialize_map(args);
    int msgID = generateMessageID();
    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

    for(auto &client : clients){

        if(!include_sender && client.first == any_cast<string>(args["cid"])) continue;

        for(const string &p : packs){
            
            SignedMessage signed_message = sign_message(p, mangePack->get_crypto_pack(getClientBySocketID(client.second)).get_x3dh_keys());
            auto signed_buffer = signed_message.serialize();
            string buffer_encrypted = to_hex(mangePack->get_crypto_pack(getClientBySocketID(client.second)).get_send_ratchet().encrypt(signed_buffer));

            uint32_t len = htonl(buffer_encrypted.size());
            send(client.second, &len, sizeof(len), 0);
            ssize_t bytes_sent = send(client.second, buffer_encrypted.c_str(), buffer_encrypted.size(), MSG_NOSIGNAL);
            if(bytes_sent < 0){
                if (baseLogActivated) printc(RED, "Send Failed\n");
            }
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
    addRoute("join", [this](map<string, any> args) {
        
        if(!args.count("room_id")){
            if (baseLogActivated) printcb(RED, "Missing 'room_id' in args.\n");
            return;
        }
        

        std::string room_id = any_cast<string>(args["room_id"]);
        std::string cid = any_cast<string>(args["cid"]);
        int client_socket = clients[cid];
        
        if(rooms.find(stoi(room_id)) == rooms.end()){
            if (baseLogActivated) printcb(RED, "Room with ID %s not found.\n", room_id.c_str());
            return;
        }

     } );
    addRoute("leave", [this](map<string, any> args) {
        if(!args.count("room_id")){
            if (baseLogActivated) printcb(RED, "Missing 'room_id' in args.\n");
            return;
        }

        std::string room_id = any_cast<string>(args["room_id"]);
        std::string cid = any_cast<string>(args["cid"]);
        
        if(rooms.find(stoi(room_id)) == rooms.end()){
            if (baseLogActivated) printcb(RED, "Room with ID %s not found.\n", room_id.c_str());
            return;
        }

        if(!rooms[stoi(room_id)].clientLeave(cid)){
            if (baseLogActivated) printcb(RED, "Client %s not found in room %s.\n", cid.c_str(), room_id.c_str());
            return;
        }
    } );

    addRoute("createRoom", [this](map<string, any> args) {
        
        if(!args.count("room_name")){
            if (baseLogActivated) printcb(RED, "Missing 'room_name' in args.\n");
            return;
        }
        std::string room_name = any_cast<string>(args["room_name"]);
        int unique_id = stoi(any_cast<string>(args["unique_id"]));
        std::string cid = any_cast<string>(args["cid"]);

        int room_id;
        if(unique_id > -1){
            if(!addRoom(unique_id,room_name)){
                if (baseLogActivated) printcb(RED, "Error creating new room with unique_id %d.\n", unique_id);
                map<string, any> error_response; 
                error_response["error"] = "Error creating new room with unique_id " + to_string(unique_id);
                error_response["cid"] = cid;
                // send error response to client
                sendMessageToClient("createRoom", cid, error_response);
                return;
            }; // Error creating new room
            room_id = unique_id;
        }else{
            room_id = addRoom(room_name); // Create new room
        }

        

        rooms[room_id].clientJoin(cid, clients[cid]); // add the owner to the room

        map<string, any> response; 

        response["room_id"] = room_id;
        response["room_name"] = room_name;

        sendMessageToClient("createRoom", cid, response);
    });
}

string SockumServer::getClientBySocketID(int sid)
{
    for (auto it = clients.begin(); it != clients.end(); ++it)
    {
        if(it->second == sid) return it->first;
    }

    return CLIENT_NOT_FOUND;
}

/* This function generate id for the Class Client */
string generateClientId()
{
    string cid;
    for (int i = 0; i < 8; ++i) {cid += chars[rand() % chars.length()];}
    return cid;
}

