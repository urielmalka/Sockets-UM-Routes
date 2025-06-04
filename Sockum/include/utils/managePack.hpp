/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief Utility class for managing and assembling chunked network messages.
 * 
 * The ManagePack class is responsible for handling the fragmentation and reassembly of large messages
 * sent over a network. It splits messages into smaller chunks based on a maximum size limit and
 * reassembles them using a message ID and client identifier.
 * 
 * Core functionalities include:
 * - Splitting long messages into chunks with associated message IDs (`chunk_string_for_network`)
 * - Reconstructing original messages from received chunks (`manegePack`, `getPack`)
 * - Tracking message chunks per client ID
 * - Managing active client IDs in the system (`add_cid`, `remove_cid`)
 */


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
        
        /**
         * @brief Splits a string into chunks suitable for network transmission.
         * @param pack The full message to be split.
         * @param message_id The unique identifier for this message, used to track chunks.
         * @return A vector of string chunks, each of size <= maxPack.
         */
        vector<string> chunk_string_for_network(const string& pack, int message_id);


        bool add_cid(const string& cid);
        bool remove_cid(const string& cid);

};

#endif