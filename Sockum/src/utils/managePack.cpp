#include "utils/managePack.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>


int extractIntBeforePipe(const std::string& input) {

    size_t pipePos = input.find('|');
    if (pipePos == std::string::npos) {
        throw std::invalid_argument("No '|' found in the input string.");
    }

    std::string numberPart = input.substr(0, pipePos);
    return std::stoi(numberPart); 
}


ManagePack::ManagePack(){}
ManagePack::ManagePack(int maxP) : maxPack(maxP) {}

ManagePack::~ManagePack(){}

bool ManagePack::manegePack(const string& cid, const string& pack, int& message_id)
{

     

    if (!network_packs.count(cid) || pack.empty()) 
        return false;

    int messageId = extractIntBeforePipe(pack);
    message_id = messageId;

    size_t pipePos = pack.find('|');
    if (pipePos == std::string::npos) {
        throw std::invalid_argument("No '|' found in the input string.");
    }

    // Append pack without the first character
    network_packs[cid][messageId] += pack.substr(pipePos + 1);

    // Return true if the first character is '1'
    return pack.back() == '~';
}


string ManagePack::getPack(const string& cid, int messageId)
{
    if(network_packs.count(cid))
    {   
        string temp = network_packs[cid][messageId];
        network_packs[cid].erase(messageId);
        return temp;
    }else{
        printc(RED,"CID: %s not exist in ManagePack\n", cid.c_str());
        return "";
    }
}


vector<string> ManagePack::chunk_string_for_network(const string& pack, int message_id)
{
    vector<string> result;
    string starter = to_string(message_id) + "|";
    size_t header_len = starter.length();
    size_t content_max_len = maxPack - header_len;

    for(size_t i = 0; i < pack.length(); i += content_max_len)
    {
        string chunk_data = pack.substr(i, content_max_len);
        result.push_back(starter + chunk_data);
    }

    return result;
}


bool ManagePack::add_cid(const string& cid) 
{ 
    if(!network_packs.count(cid))
    {
        network_packs[cid] = {};
        return true;
    }

    return false;
};
bool ManagePack::remove_cid(const string& cid)
{
    size_t remove = network_packs.erase(cid);
    if(remove > 0) return true;
    else return false;
};


void ManagePack::add_crypto_pack(const string& cid)
{
    if(!crypto_packs.count(cid))
    {
        CryptoPack crypto_pack(true);
        crypto_packs[cid] = crypto_pack;
    }
}

void ManagePack::set_shared_secret(const string& cid, const X3DHKeys& keys)
{
    crypto_packs[cid].set_shared_secret(keys);
}