#include "utils/managePack.hpp"

ManagePack::ManagePack(){}
ManagePack::ManagePack(int maxP) : maxPack(maxP) {}

ManagePack::~ManagePack(){}

bool ManagePack::manegePack(const string& cid, const string& pack)
{
    if (!network_packs.count(cid) || pack.empty()) 
        return false;

    // Append pack without the first character
    network_packs[cid] += pack.substr(1);
    
    // Return true if the first character is '1'
    return pack[0] == '1';
}


string ManagePack::getPack(const string& cid)
{
    if(network_packs.count(cid))
    {   
        string temp = network_packs[cid];
        network_packs[cid] = "";
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


vector<string> ManagePack::chunk_string_for_network(const string& pack)
{
    vector<string> result;
    for(size_t i=0; i < pack.length() ; i += maxPack )
    {
        cout << i + maxPack << endl;
        bool isLastChunk = (i + maxPack >= pack.length());

        string s = (isLastChunk ? "1" : "0") + pack.substr(i, maxPack);
        cout << s.length() << s.size() << endl;

        result.push_back((isLastChunk ? "1" : "0") + pack.substr(i, maxPack));

    }

    return result;
}

bool ManagePack::add_cid(const string& cid) 
{ 
    if(!network_packs.count(cid))
    {
        network_packs[cid] = "";
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