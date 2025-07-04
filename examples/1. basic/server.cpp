#include <Sockum.hpp>


int main()
{
    SockumServer * server = new SockumServer();

    server->addRoute("hello_client", [](map<string, any>& args) {
        // Handle the request
        std::string client_id = any_cast<std::string>(args["cid"]);
        std::string message = any_cast<std::string>(args["message"]);
        
        // Log the received message
        std::cout << "Received from client " << client_id << ": " << message << std::endl;
    });

    server->run();
    
    return 0;
}