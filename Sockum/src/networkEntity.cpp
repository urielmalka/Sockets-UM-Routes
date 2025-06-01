#include "networkEntity.hpp"
#include <cstdint>
#include <arpa/inet.h> 

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




bool SockumNetworkEntity::recv_all(int socket, char* buffer, size_t& size) {

    uint32_t msg_len_network;
    size_t len_received = 0;
    while (len_received < sizeof(msg_len_network)) {
        int bytes = recv(socket, reinterpret_cast<char*>(&msg_len_network) + len_received, sizeof(msg_len_network) - len_received, 0);
        if (bytes <= 0) return false;
        len_received += bytes;
    }

    uint32_t msg_len = ntohl(msg_len_network);


    if (msg_len > size) {
        return false; // overflow 
    }

    size_t total_received = 0;
    while (total_received < msg_len) {
        int bytes = recv(socket, buffer + total_received, msg_len - total_received, 0);
        if (bytes <= 0) return false;
        total_received += bytes;
    }

    size = msg_len;
    return true;
}


