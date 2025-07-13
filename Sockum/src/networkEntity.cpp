#include "networkEntity.hpp"
#include <cstdint>
#include <arpa/inet.h> 

SockumNetworkEntity::SockumNetworkEntity()
{
    mangePack = new ManagePack();
};

SockumNetworkEntity::~SockumNetworkEntity(){};

bool SockumNetworkEntity::recv_all(int socket, char* buffer, size_t& size) {

    uint32_t msg_len_network;
    size_t len_received = 0;
    while (len_received < sizeof(msg_len_network)) {
        int bytes = recv(socket, reinterpret_cast<char*>(&msg_len_network) + len_received, sizeof(msg_len_network) - len_received, 0);
        if (bytes <= 0) return false;
        len_received += bytes;
    }

    uint32_t msg_len = ntohl(msg_len_network);


    if (msg_len > size) {
        std::cerr << "Error: Message size exceeds buffer size.\n";
        std::cerr << "Buffer size: " << size << ", Message size: " << msg_len << "\n";
        return false; // overflow 
    }

    size_t total_received = 0;
    while (total_received < msg_len) {
        int bytes = recv(socket, buffer + total_received, msg_len - total_received, 0);
        if (bytes <= 0) return false;
        total_received += bytes;
    }

    size = msg_len;
    return true;
}


bool SockumNetworkEntity::addRoom(int room_id, const string& room_name)
{
    if (rooms.find(room_id) != rooms.end()) {
        printcb(RED, "Room with ID %d already exists.\n", room_id);
        return false;
    }
    
    rooms[room_id] = Room(room_name);
    return true;
}


int SockumNetworkEntity::addRoom(const string& room_name)
{
    static int room_id = 0;
    rooms[room_id] = Room(room_name);
    room_id++;
    return room_id - 1; // return the ID of the last room that we create right now
}


bool SockumNetworkEntity::removeRoom(int room_id)
{
    auto it = rooms.find(room_id);
    if (it == rooms.end()) {
        printcb(RED, "Room with ID %d does not exist.\n", room_id);
        return false;
    }

    rooms.erase(it);
    printcb(GREEN, "Room with ID %d removed successfully.\n", room_id);
    return true;
}


void SockumNetworkEntity::logGet(map<string, any> &args, std::string route)
{
    if(!logActivated) return;

    printcu(YELLOW, "GET: %s",route.c_str());
    printc(YELLOW, "\n\t{\n\t\t");
    for(auto &pair : args)
    {
        printc(YELLOW, "\"%s\": \"%s\" \n\t\t",pair.first.substr(0,50).c_str(), any_cast<string>(pair.second).substr(0,50).c_str());
    }
    printc(YELLOW, "\b\b\b\b\b\b\b\b}\n");
}

void SockumNetworkEntity::logSend(map<string, any> &args)
{
    if(!logActivated) return;

    printcu(GREEN, "SEND:");
    printc(GREEN, "\n\t{\n\t\t");
    for(auto &pair : args)
    {
        printc(GREEN, "\"%s\": \"%s\" \n\t\t",pair.first.substr(0,50).c_str(), any_cast<string>(pair.second).substr(0,50).c_str());
    }
    printc(GREEN, "\b\b\b\b\b\b\b\b}\n");
}