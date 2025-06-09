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

    public:
        Room(const std::string &room_name);
        ~Room();

        bool clientJoin();
        bool clientLeave();
};

#endif