#ifndef MANEGEPACK_HPP
#define MANAGEPACK_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "../external/UColor_C/include/color.h"

using namespace std;

class ManagePack
{
    private:
        map<string, string> network_packs;
        int maxPack = 1023; // N - 1 

    public:
        ManagePack();
        ManagePack(int maxP);
        ~ManagePack();

        bool manegePack(const string& cid, const string& pack);
        string getPack(const string& cid);
        vector<string> chunk_string_for_network(const string& pack);

        bool add_cid(const string& cid);
        bool remove_cid(const string& cid);

};

#endif