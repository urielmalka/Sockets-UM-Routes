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
        map<string, map<int,string>> network_packs;
        int maxPack = 32768; 

    public:
        ManagePack();
        ManagePack(int maxP);
        ~ManagePack();

        bool manegePack(const string& cid, const string& pack, int& message_id);
        string getPack(const string& cid, int messageId);
        vector<string> chunk_string_for_network(const string& pack, int message_id);

        bool add_cid(const string& cid);
        bool remove_cid(const string& cid);

};

#endif