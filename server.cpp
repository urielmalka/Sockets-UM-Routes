#include "server.hpp"


Server::Server()
{
    server_port = 8080;
    max_connection = 128;
    initServer();
}

Server::Server(int PORT)
{
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

    cout << "Server is running on PORT: " << server_port << endl;
}