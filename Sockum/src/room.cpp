#include "room.hpp"

Room::Room(const std::string &room_name)
{
    this->room_name = room_name;
}

Room::~Room(){}

bool Room::clientJoin(std::string client_id, int client_socket)
{
    if (clients.find(client_id) != clients.end()) {
        return false; // Client already in the room
    }
    
    clients[client_id] = client_socket;
    return true;
}
    
bool Room::clientLeave(std::string client_id)
{
    auto it = clients.find(client_id);
    if (it != clients.end()) {
        clients.erase(it);
        return true;
    }
    return false;
}