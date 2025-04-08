#include <Sockum/Sockum.hpp>


int main()
{
    SockumClient *c2 = new SockumClient();

    

    thread t([c2]() { c2->run(); });
    t.detach();
    
    map<string, any> args;
    string meesgae;

    meesgae = "Hello from client2 start\n";

    for (int i=0; i < 100 ; i++){
        meesgae = meesgae + "Hello from client2 " + to_string(i) + "\n";
    }
    args["message"] = meesgae;
    //std::cout << meesgae << endl;
    //std::cout << meesgae.size() << endl;

    c2->route("/sendMessageToAll",args);
    
    while (1)
    {
        /* code */
    }
    

    c2->disconnect();
    
    return 0;
}