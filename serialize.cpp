#include "serialize.hpp"


string serialize_cast(any a) {
    if (a.type() == typeid(string)) {
        return any_cast<string>(a);
    } else if (a.type() == typeid(const char*)) {
        return string(any_cast<const char*>(a));
    } else if (a.type() == typeid(int)) {
        return to_string(any_cast<int>(a));
    } else if (a.type() == typeid(double)) {
        return to_string(any_cast<double>(a));
    } else if (a.type() == typeid(bool)) {
        return any_cast<bool>(a) ? "true" : "false";
    }
    // add more types if needed
    return "<unsupported>";
}

string serialize_map(const map<string,any>& args)
{
    if (args.empty()) return "";

    ostringstream buffer;

    for(const auto& pair : args)
    { 
        buffer << 
        pair.first << 
        "/:" << 
        ( pair.second.has_value() ? serialize_cast(pair.second) : "" ) << 
        "/#"; 
    }

    return buffer.str();
}


int serialize_str(const string s, map<string,any> * args)
{

    string first,second;

    size_t pos = 0;
    

    while (pos < s.size())
    {
        size_t end = s.find(CELL_MIDDLE_VALUE, pos);
        if(end == -1) return ERROR_SERIALIZE_PACK;
        first = s.substr(pos, end - pos);
        pos = end + CELL_MIDDLE_VALUE.length();
    
        end = s.find(CELL_LAST_VALUE, pos);
        if(end == -1) return ERROR_SERIALIZE_PACK;

        second = s.substr(pos, end - pos);
        pos = end + CELL_LAST_VALUE.length();

        (*args)[first] = static_cast<any>(second);
    }

    return  SUCCESS_SERIALIZE_PACK;
}

int serialize_route(const string s, string *rout)
{
    size_t index = s.find(AT_SIGN_ROUTE);

    if(index == -1) return ERROR_ROUTE;

    *rout = s.substr(0,index);

    return index + 1; // + 1 for '@' 
}