#ifndef MANEGEPACK_HPP
#define MANAGEPACK_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class ManagePack
{
    private:
        map<int, string> network_packs;

    public:
        ManagePack();
        ~ManagePack();

        bool manegePack(int sid, const string& pack);
        vector<string> chunk_string_for_network(const string& pack);

        bool add_sid(int sid);
        bool remove_sid(int sid);

    };

ManagePack::ManagePack(){}

ManagePack::~ManagePack(){}

bool ManagePack::manegePack(int sid, const string& pack)
{
    return true;
}


vector<string> ManagePack::chunk_string_for_network(const string& pack)
{

}

bool ManagePack::add_sid(int sid) 
{ 
    if(!network_packs.count(sid))
    {
        network_packs[sid] = "";
        return true;
    }

    return false;
};
bool ManagePack::remove_sid(int sid)
{
    size_t remove = network_packs.erase(sid);
    if(remove > 0) return true;
    else return false;
};
#endif