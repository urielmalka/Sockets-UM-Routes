#ifndef MANEGEPACK_HPP
#define MANAGEPACK_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <color.h>

using namespace std;

class ManagePack
{
    private:
        map<string, string> network_packs;

    public:
        ManagePack();
        ~ManagePack();

        bool manegePack(const string& sid, const string& pack);
        string getPack(const string& sid);
        vector<string> chunk_string_for_network(const string& pack);

        bool add_cid(const string& sid);
        bool remove_cid(const string& sid);

    };

ManagePack::ManagePack(){}

ManagePack::~ManagePack(){}

bool ManagePack::manegePack(const string& sid, const string& pack)
{
    if(network_packs.count(sid))
    {   
        return true;
    }else{
        return false;
    }

}

string ManagePack::getPack(const string& sid)
{
    if(network_packs.count(sid))
    {   
        return network_packs[sid];
    }else{
        printc(RED,"SID: %s not exist in ManagePack\n", sid.c_str());
        return "";
    }
}

vector<string> ManagePack::chunk_string_for_network(const string& pack)
{

}

bool ManagePack::add_cid(const string& sid) 
{ 
    if(!network_packs.count(sid))
    {
        network_packs[sid] = "";
        return true;
    }

    return false;
};
bool ManagePack::remove_cid(const string& sid)
{
    size_t remove = network_packs.erase(sid);
    if(remove > 0) return true;
    else return false;
};
#endif