#include "client.hpp"
#include "utils/sockumfile.hpp"
#include <thread>   
#include <chrono> 

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
    int message_id;

    while (1)
    {
        char buffer[MAX_CHUNK_SIZE] = {0};
        size_t actual_size = MAX_CHUNK_SIZE;

        if (!recv_all(serverSocket,buffer, actual_size)) break;

        string strBuffer(buffer,actual_size);

        /*if(isCryptp){
            string de_buffer = decrypt(buffer);
            strncpy(buffer,de_buffer.c_str(),BUFFER_SIZE - 1);
        }*/

        if(!mangePack->manegePack("local",strBuffer,message_id)) continue;

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
    routes[route] = funcRoute;
    return this;
}

SockumClient* SockumClient::addFileRoute(string route)
{
    routes[route] = [this](map<string, any> args) { routeFileProcess(args); };;
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

    int msgID = generateMessageID();

    vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

    for(const string &p : packs){
        uint32_t len = htonl(p.size());
        send(serverSocket, &len, sizeof(len), 0);
        send(serverSocket, p.c_str(), p.size(), 0);
    }

    return this;
}

SockumClient* SockumClient::routeFile(string route, string path)
{
    fs::path filePath(path);
    string filename = filePath.filename().string();

    std::uintmax_t filesize = get_file_size(path);
    std::uintmax_t numChunks = (filesize + 204800 - 1) / 204800;

    map<string, any> args;

    for(int i=0; i < numChunks ; i++)
    {
        args["filename"] = filename;
        args["content"] = readChunkFromFile(path, i);
        args["size"] = filesize;
        args["load"] = i * 204800;
        string buffer = route + "@" + serialize_map(args);
        int msgID = generateMessageID();
        vector<string> packs = mangePack->chunk_string_for_network(buffer,msgID);

        for(const string &p : packs){
            uint32_t len = htonl(p.size());
            send(serverSocket, &len, sizeof(len), 0);
            send(serverSocket, p.c_str(), p.size(), 0);
        }

    }

    return this;
}

void SockumClient::routeFileProcess(map<string,any>& args)
{

    string filename = "new/" + any_cast<string>(args["filename"]);
    string content = any_cast<string>(args["content"]);
    std::uintmax_t file_size = std::stoull(any_cast<std::string>(args["size"]));
    std::uintmax_t file_load = std::stoull(any_cast<std::string>(args["load"]));

    appendChunkToFile(filename, content);
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