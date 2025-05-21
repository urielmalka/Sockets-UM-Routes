#include "networkEntity.hpp"

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