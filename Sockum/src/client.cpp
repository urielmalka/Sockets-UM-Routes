#include "client.hpp"
#include "utils/sockumfile.hpp"
#include <thread>   
#include <chrono> 
#include <filesystem>
#include <arpa/inet.h> 
#include <regex>

SockumClient::SockumClient(std::string server_ip, int PORT)
{
    if(PORT < 1) throw runtime_error("PORT must be greater than zero.");
    server_port = PORT;
    initClient(server_ip);
}

SockumClient::~SockumClient()
{
    delete crypto_pack;
    close(serverSocket);
}

void SockumClient::initClient(std::string server_ip)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server_port);

    if(server_ip.empty()) {
        serverAddress.sin_addr.s_addr = INADDR_ANY;
    }else{
        serverAddress.sin_addr.s_addr = inet_addr(server_ip.c_str());
    }

    crypto_pack = new CryptoPack();

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
            send(serverSocket, crypto_pack->get_x3dh_keys().identity_pub.data(), 32, 0);
            send(serverSocket, crypto_pack->get_x3dh_keys().signed_prekey_pub.data(), 32, 0);
            send(serverSocket, crypto_pack->get_x3dh_keys().one_time_pub.data(), 32, 0);

            std::unique_ptr<X3DHKeys> x3dh_keys = std::make_unique<X3DHKeys>();

            recv(serverSocket, x3dh_keys->identity_pub.data(), crypto_kx_PUBLICKEYBYTES, MSG_WAITALL);
            recv(serverSocket, x3dh_keys->signed_prekey_pub.data(), crypto_kx_PUBLICKEYBYTES, MSG_WAITALL);
            recv(serverSocket, x3dh_keys->one_time_pub.data(), crypto_kx_PUBLICKEYBYTES, MSG_WAITALL);

            crypto_pack->set_shared_secret(*x3dh_keys);

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
    int message_id;

    while (1)
    {
        char buffer[MAX_CHUNK_SIZE * 2] = {0};
        size_t actual_size = MAX_CHUNK_SIZE * 2;

        if (!recv_all(serverSocket,buffer, actual_size)) break;

        string strBuffer(buffer,actual_size);

        if(logActivated) printc(BLUE, "Received message ID: %d, Buffer: %s [SIZE: %zu]\n", message_id, strBuffer.substr(0, 50).c_str(), strBuffer.size());

        string decrypted = crypto_pack->get_receive_ratchet().decrypt(from_hex(strBuffer));

        if(logActivated) printc(BLUE, "Decrypted message: %s [SIZE: %zu]\n", decrypted.substr(0, 50).c_str(), decrypted.size());

        if(!mangePack->manegePack("local",decrypted,message_id)) continue;

        string packBuffer = mangePack->getPack("local",message_id);

        string t = "~";
        vector<string> result = splitByDelimiter(packBuffer, t);

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

    printcb(RED, "Client %s is disconnect now\n",client_id.c_str());
    
    close(serverSocket);
    
}

SockumClient* SockumClient::addRoute(string route, function<void(map<string, any>)> funcRoute)
{
    if (routes.find(route) != routes.end()) {
        printci(RED, "Warning: Route already exists. Replacing route: %s\n", route.c_str());
    } else {
        printci(BLUE, "Route Added: %s\n", route.c_str());
    }
    routes[route] = funcRoute;
    return this;
}

SockumClient* SockumClient::addFileRoute(string route, const string path, function<void(map<string, any>)> funcRoute)
{
    printci(BLUE,"File Route Added: %s\n",route.c_str());
    routes[route] = [this, path, funcRoute](map<string, any> args) {
            routeFileProcess(args, path); 
            if (funcRoute) {
                funcRoute(args);
            }
        };
    return this;
}

template <typename T>
SockumClient* SockumClient::addRoute(string route, function<void(T, map<string, any>)> funcRoute)
{
    if (routes.find(route) != routes.end()) {
        printci(RED, "Warning: Route already exists. Replacing route: %s\n", route.c_str());
    } else {
        printci(BLUE, "Route Added: %s\n", route.c_str());
    }
    routes[route] = funcRoute;
    return this;
}

SockumClient* SockumClient::route(string route, map<string, any>& args)
{
    while (!clientReady);
    
    string buffer = route + "@" + serialize_map(args);

    int msgID = generateMessageID();

    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

    for(const string &p : packs){
        string buffer_encrypted = to_hex(crypto_pack->get_send_ratchet().encrypt(p));
        uint32_t len = htonl(buffer_encrypted.size());
        send(serverSocket, &len, sizeof(len), 0);
        send(serverSocket, buffer_encrypted.c_str(), buffer_encrypted.size(), 0);
    }

    return this;
}

SockumClient* SockumClient::routeFile(string route, string path)
{
    while (!clientReady);
    fs::path filePath(path);
    string filename = filePath.filename().string();

    std::uintmax_t filesize = get_file_size(path);
    std::uintmax_t numChunks = (filesize + FILE_CHUNK - 1) / FILE_CHUNK;

    string fileId = "." + generateFileId();

    map<string, any> args;

    for(int i=0; i < numChunks ; i++)
    {
        args["filename"] = filename + fileId;
        args["content"] = readChunkFromFile(path, i);
        args["size"] = filesize;
        args["load"] = (1+i) * FILE_CHUNK > filesize ? filesize : i * FILE_CHUNK;
        args["finished"] =  (1+i) * FILE_CHUNK > filesize;


        string buffer = route + "@" + serialize_map(args);
        int msgID = generateMessageID();
        vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

        for(const string &p : packs){

            string buffer_encrypted = to_hex(crypto_pack->get_send_ratchet().encrypt(p));

            uint32_t len = htonl(buffer_encrypted.size());

            send(serverSocket, &len, sizeof(len), 0);
            send(serverSocket, buffer_encrypted.c_str(), buffer_encrypted.size(), 0);
        }

    }

    return this;
}

void SockumClient::routeFileProcess(map<string,any>& args, const string &save_path)
{
    string filename;
    if(!save_path.empty()){
        std::error_code ec;
        if (!fs::exists(save_path)) {
            fs::create_directories(save_path, ec);
            if (ec) {
                std::cerr << "⚠️ Failed to create directory: " << save_path << " (" << ec.message() << ")\n";
                return;
            }
        }
        filename = save_path + "/";
    }
    filename += any_cast<string>(args["filename"]);

    string content = any_cast<string>(args["content"]);
    std::uintmax_t file_size = std::stoull(any_cast<std::string>(args["size"]));
    std::uintmax_t file_load = std::stoull(any_cast<std::string>(args["load"]));

    appendChunkToFile(filename, content);


    if (args.find("finished") != args.end() 
                        && 
                        (any_cast<std::string>(args["finished"]) == "true" || 
                        any_cast<std::string>(args["finished"]) == "1")) 
    {
        size_t last_dot = filename.rfind('.');
        if (last_dot != std::string::npos) {
            std::string base_filename = filename.substr(0, last_dot);
            std::string new_filename = base_filename;
            int counter = 1;
            while (fs::exists(new_filename)) {
                new_filename = base_filename + " (" + std::to_string(counter++) + ")";
            }
            fs::rename(filename, new_filename); 
            //std::cout << "File finalized: " << new_filename << std::endl;
        }
    }

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
    addRoute("setCrypto", [this](map<string, any> args) {
        
    });

    addRoute("setId", [this](map<string, any> args) {
        setClientId(args); 
        clientReady = true;
    });

    addRoute("createRoom", [this](map<string, any> args) {
        
        if(args.count("error")){
            printcb(RED, "Error creating room: %s\n", any_cast<string>(args["error"]).c_str());
            return;
        }

        std::string room_id = any_cast<string>(args["room_id"]);
        std::string room_name = any_cast<string>(args["room_name"]);

        join(stoi(room_id), room_name);

    } );
}

bool SockumClient::createRoom(std::string room_name, int unique_id)
{
    map<string, any> args;
    args["room_name"] = room_name;
    args["unique_id"] = unique_id;
    route("createRoom", args);
    return true;
}


bool SockumClient::join(int room_id, std::string room_name)
{
    map<string, any> args;
    args["room_id"] = room_id;
    route("join", args);

    if(addRoom(room_id, room_name) && room_name.length() > 0){
        printcb(GREEN, "Join to %s\n",room_name.c_str());
    }
    return true;
}

bool SockumClient::leave(int room_id, std::string room_name)
{
    map<string, any> args;
    args["room_id"] = room_id;
    route("leave", args);

    if(removeRoom(room_id) && room_name.length() > 0){
        printcb(YELLOW, "Leave from %s\n",room_name.c_str());
    }
    return true;
}