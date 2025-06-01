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
/*
std::string mapToString(const std::map<int, std::string>& m) {
    std::ostringstream oss;
    auto it = m.find(-1);

    for (const auto& [key, value] : m) {
        if (key != -1) {
            oss << value;
        }
    }

    if (it != m.end()) {
        oss << it->second;
    }

    return oss.str();
}
*/


ManagePack::ManagePack(){}
ManagePack::ManagePack(int maxP) : maxPack(maxP) {}

ManagePack::~ManagePack(){}

bool ManagePack::manegePack(const string& cid, const string& pack, int& message_id)
{

     

    if (!network_packs.count(cid) || pack.empty()) 
        return false;

    int messageId = extractIntBeforePipe(pack);
    message_id = messageId;

    std::cout << "message_id: " << message_id << endl;

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


/*
string ManagePack::chunk_string_for_network(const string& pack)
{
    string result;
    for(size_t i=0; i < pack.length() ; i += maxPack )
    {
        if(i + maxPack >= pack.length())
        {
            result = result + '1' + pack.substr(i, maxPack);
        }else{
            result = result + '0' + pack.substr(i, maxPack);
        }

    }

    return result;
}
*/


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