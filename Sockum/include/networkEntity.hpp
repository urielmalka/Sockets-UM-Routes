/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief Base network entity class for Sockum client and server implementations.
 * 
 * The SockumNetworkEntity class provides shared infrastructure for both SockumClient and SockumServer.
 * It handles socket setup, message ID generation, encryption/decryption logic,
 * and packet management through the ManagePack utility.
 * 
 * Core functionalities include:
 * - Initialization and cleanup of network socket
 * - Generating unique message IDs (thread-safe)
 * - Optional encryption and decryption support for sent/received messages
 * - Receiving full messages using recv_all
 * - Access to the ManagePack utility for chunking large messages
 */



#ifndef NETWORKENTITY_HPP 
#define NETWORKENTITY_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <map>
#include <any>
#include <string.h>
#include <functional>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <ctime>
#include <list>
#include <atomic>
#include <mutex>

#include "utils/managePack.hpp"
#include "room.hpp"

#include "../external/UColor_C/include/color.h"

#define MAX_CHUNK_SIZE 32768

using namespace std;

class SockumNetworkEntity{

    private:
        std::mutex id_mutex; 
        int currentMessageID = 0;

    protected:
        int serverSocket;
        int server_port;

        bool logActivated = false;
        bool baseLogActivated = true;

        map<int,Room> rooms;

        bool recv_all(int socket, char* buffer, size_t& size);

        ManagePack* mangePack;

        int generateMessageID() {
            std::lock_guard<std::mutex> lock(id_mutex);
            return currentMessageID++;
        }    

        void logGet(map<string, any> &args, std::string route);
        void logSend(map<string, any> &args);

    public:
        
        SockumNetworkEntity();
        ~SockumNetworkEntity();

        bool addRoom(int room_id, const string& room_name);
        int addRoom(const string& room_name);
        bool removeRoom(int room_id);

        void setLogActivated(bool activated) {
            logActivated = activated;
        }

        void setBaseLogActivated(bool activated) {
            baseLogActivated = activated;
        }
        
        

};

#endif