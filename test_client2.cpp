#include <Sockum/Sockum.hpp>


int main()
{
    Client *c2 = new Client();

    

    thread t([c2]() { c2->run(); });
    t.detach();
    
    map<string, any> args;

    args["message"] = "Hello from client2";

    c2->route("/sendMessageToAll",args);


    args["message"] = "TOT from client2";


    c2->route("/sendMessageToAll",args);

    while (1)
    {
        /* code */
    }
    
    
    return 0;
}