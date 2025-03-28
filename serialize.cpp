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
   
    ostringstream buffer;

    if (args.empty()){
        buffer << TILTA;
        return buffer.str();
    };


    for(const auto& pair : args)
    { 
        buffer << 
        pair.first << 
        "/:" << 
        ( pair.second.has_value() ? serialize_cast(pair.second) : "" ) << 
        "/#"; 
    }

    return buffer.str() + TILTA;
}


int serialize_str(const string& s, map<string,any> * args)
{
    string tempS,first,second;

    size_t pos = 0;

    tempS = s;
    
    size_t index = s.find(TILTA);
    if(index != -1) tempS = s.substr(0,index);

    while (pos < tempS.size())
    {
        size_t end = tempS.find(CELL_MIDDLE_VALUE, pos);
        if(end == -1) return ERROR_SERIALIZE_PACK;
        first = tempS.substr(pos, end - pos);
        pos = end + CELL_MIDDLE_VALUE.length();
    
        end = tempS.find(CELL_LAST_VALUE, pos);
        if(end == -1) return ERROR_SERIALIZE_PACK;

        second = tempS.substr(pos, end - pos);
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


vector<string> splitByDelimiter(const string& str, const string& delimiter) {
    vector<string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != string::npos) {
        if(str.substr(start, end - start).size() > 0)
            result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    if(str.substr(start).size() > 0)
        result.push_back(str.substr(start));

    return result;
}