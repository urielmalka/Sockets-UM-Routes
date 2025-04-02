#include <Sockum/Sockum.hpp>


void sendMessageToAll(map<string, any> args)
{
    printc(BLUE, "sendMessageToAll\n");
    for(auto &pair: args){
        printc(YELLOW, "%s : %s\n", pair.first.c_str(), any_cast<string>(pair.second).c_str());
    }
}


int main()
{
    Client *c1 = new Client();

    c1->addRoute("/sendMessageToAll",sendMessageToAll);

    c1->run();

    printc(YELLOW, "ByeBye\n");
    
    return 0;
}