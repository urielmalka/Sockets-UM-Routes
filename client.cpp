#include "client.hpp"

Client::Client()
{
    server_port = 8080;
    initClient();
}

Client::Client(int PORT)
{
    if(PORT < 1) throw runtime_error("PORT must be greater than zero.");
    server_port = PORT;
    initClient();
}

Client::~Client()
{
    close(serverSocket);
}

void Client::initClient()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connectClient();
    coreRoutes();
    
}

void Client::connectClient()
{
    int count_try_connection = 0, is_connected;
    while (count_try_connection < 5)
    {
        is_connected = connect(serverSocket, (struct sockaddr*)&serverAddress,sizeof(serverAddress));
        if(is_connected == 0 ) {
            printc(GREEN,"Client connect to port %d.\n",server_port);
            break;
        };
        printc(RED,"(%d) : Client failed to connect to port %d\n",count_try_connection,server_port);
        count_try_connection ++;
        this_thread::sleep_for(chrono::seconds(5));

    }
    
}

void Client::listenerRoutes()
{   

    string route;
    map<string, any> pack;

    while (1)
    {
        char buffer[BUFFER_SIZE] = {0};

        int bytes_received = recv(serverSocket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) break;

        /*printc(YELLOW,"FROM SERVER: %s\n",buffer);

        int index = serialize_route(buffer, &route);
        
        if(index != ERROR_ROUTE);

        memmove(buffer, buffer + index, BUFFER_SIZE);

        if(serialize_str(buffer, &pack) != SUCCESS_SERIALIZE_PACK) return;

        try{
            auto func = routes.at(route);
            func(pack);
        }catch(const out_of_range &e){
            printc(RED,"Error: ");
            cout << route << " not found." << endl;
        }
        
        memmove(buffer, buffer - index, BUFFER_SIZE);
        pack.clear();
        */
        string t = "~";
        vector<string> result = splitByDelimiter(buffer, t);

        for (const string& s : result) {
            
            char* c = new char[s.size() + 1];

            strcpy(c, s.c_str());
            //printci(MAGENTA, "%s\n",c);

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

void Client::addRoute(string route, function<void(map<string, any>)> funcRoute)
{
    routes[route] = funcRoute;
}

void Client::route(string route, map<string, any>& args)
{
    string buffer = route + "@" + serialize_map(args);
    printc(GREEN,"CLIENT_SEND: %s\n",buffer.c_str());
    send(serverSocket, buffer.c_str(), buffer.size(), 0);
}

void Client::run(){
    listenerRoutes();
}


void Client::setClientId(map<string, any>& args)
{
    client_id = any_cast<string>(args["cid"]);
    printcb(GREEN, "Client %s is connect now.\n",client_id.c_str());
}

void Client::coreRoutes()
{   
    addRoute("/setId", [this](map<string, any> args) {setClientId(args); } );
}