#ifndef NETWORKENTITY_HPP 
#define NETWORKENTITY_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <color.h>
#include <map>
#include <any>
#include <string.h>
#include <functional>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <ctime>
#include <list>

using namespace std;

class NetworkEntity{

    private:

    protected:
        int serverSocket;
        int server_port;

    public:
        
        NetworkEntity();
        ~NetworkEntity();

};

NetworkEntity::NetworkEntity(){};

NetworkEntity::~NetworkEntity(){};


#endif