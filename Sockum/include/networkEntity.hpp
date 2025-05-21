#ifndef NETWORKENTITY_HPP 
#define NETWORKENTITY_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <map>
#include <any>
#include <string.h>
#include <functional>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <ctime>
#include <list>

#include "utils/managePack.hpp"

#include "../external/UColor_C/include/color.h"

using namespace std;

class SockumNetworkEntity{

    private:

    protected:
        int serverSocket;
        int server_port;
        bool isCryptp = false;

        ManagePack* mangePack;

        function<string( const string&)> decrypt;
        function<string( const string&)> encrypt;

        void setDecrypt(function<string( const string&)> d) { decrypt = d; };
        void setEncrypt(function<string( const string&)> e) { encrypt = e; };

    public:
        
        SockumNetworkEntity();
        ~SockumNetworkEntity();

        void setCrypto(function<string( const string&)> d, function<string( const string&)> e);

};

#endif