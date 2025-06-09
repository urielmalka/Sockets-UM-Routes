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
        bool isCryptp = false;

        vector<Room> rooms;

        bool recv_all(int socket, char* buffer, size_t& size);

        ManagePack* mangePack;

        function<string( const string&)> decrypt;
        function<string( const string&)> encrypt;

        void setDecrypt(function<string( const string&)> d) { decrypt = d; };
        void setEncrypt(function<string( const string&)> e) { encrypt = e; };

        int generateMessageID() {
            std::lock_guard<std::mutex> lock(id_mutex);
            return currentMessageID++;
        }
    

    public:
        
        SockumNetworkEntity();
        ~SockumNetworkEntity();

        void setCrypto(function<string( const string&)> d, function<string( const string&)> e);

        void addRoom(const string& room_name);
        

};

#endif