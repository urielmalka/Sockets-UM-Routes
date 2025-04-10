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

#include "Sockum/utils/managePack.hpp"

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

SockumNetworkEntity::SockumNetworkEntity()
{
    mangePack = new ManagePack();
};

SockumNetworkEntity::~SockumNetworkEntity(){};

void SockumNetworkEntity::setCrypto(function<string( const string&)> decrypt, function<string( const string&)> encrypt)
{
    setDecrypt(decrypt);
    setEncrypt(encrypt);
    isCryptp = true;
}

#endif