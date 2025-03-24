#include "server.hpp"


Server::Server()
{
    server_port = 8080;
    max_connection = 128;
    initServer();
    coreRoutes();
}

Server::Server(int PORT)
{
    if(PORT < 1) throw runtime_error("PORT must be greater than zero.");
    server_port = PORT;
    max_connection = 128;
    initServer();
}

Server::Server(int PORT, int max_conn)
{
    if(max_conn < 1 || PORT < 1) throw runtime_error("Values must be greater than zero.");

    server_port = PORT;
    max_connection = max_conn;
    initServer();
}

Server::~Server(){}

void Server::initServer()
{
    serverSocket =  socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(serverSocket, (struct sockaddr*)&serverAddress,sizeof(serverAddress));

    // listening to the assigned socket
    listen(serverSocket, max_connection);
}

void Server::run()
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

        thread thread_client([this, new_socket](){
            listenerRoutes(new_socket);
        });
        thread_client.detach();

    }
}

void Server::listenerRoutes(int client_id)
{   

    string route;
    map<string, any> pack;

    printcb(GREEN, "Client %d is connect now.\n", client_id);

    while (1)
    {
        char buffer[BUFFER_SIZE] = {0};

        int bytes_received = recv(client_id, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) break;

        pack["cid"] = int(client_id);

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
        

    }

    printcb(RED, "Client %d is disconnect now.\n", client_id);
    
    close(client_id);
    
}

void Server::addRoute(string route, function<void(map<string, any>)> funcRoute)
{
    routes[route] = funcRoute;
}

void Server::route(string route, map<string, any>& args, int client_id)
{
    string buffer = route + "@" + serialize_map(args);
    printc(GREEN,"SERVER_SEND: %s\n",buffer.c_str());
    send(client_id, buffer.c_str(), buffer.size(), 0);

}

void Server::setClientId(map<string, any> & args)
{
    int client_id = any_cast<int>(args["cid"]);

    string tempId;
    map<string, any> response;

    
    tempId = generateClientId();
    
    while (clients.count(tempId) > 0)
    {
        tempId = generateClientId();
    }
    
    clients[tempId] = client_id;
    response["cid"] = tempId;
    route("/setId", response,client_id);

}

void Server::coreRoutes()
{   
    addRoute("/setId", [this](map<string, any> args) {setClientId(args); } );
}

/* This function generate id for the Class Client */
string generateClientId()
{
    string cid;
    for (int i = 0; i < 8; ++i) {cid += chars[rand() % chars.length()];}
    return cid;
}