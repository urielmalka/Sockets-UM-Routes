#ifndef ROOM_HPP
#define ROOM_HPP

#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

class Room
{
    private:    
        int roomId;
        std::map<std::string, int> clients;

        std::string room_name;

    public:
        Room(const std::string &room_name);
        ~Room();

        bool clientJoin(std::string client_id, int client_socket);
        bool clientLeave(std::string client_id);

        std::string getRoomName() const { return room_name; }
        std::map<std::string, int> getClients() const { return clients; }
        int getRoomId() const { return roomId; }
        
        
        void setRoomId(int id) { roomId = id; }
        void setRoomName(const std::string &name) { room_name = name; }
        
        void printClients() const {
            std::cout << "Clients in room " << room_name << ":\n";
            for (const auto &client : clients) {
                std::cout << "Client ID: " << client.first << ", Socket: " << client.second << "\n";
            }
        }
};

#endif