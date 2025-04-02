#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

/*

The structure of the package starts with a ROUTE, followed by a MAP object.

The ROUTE appears in the format: /exampleRoute

The "@" symbol separates the ROUTE from the MAP.

The MAP is written in the following format: KEY_1/:VALUE_1/#KEY_2/:VALUE_2/#...KEY_N/:VALUE_N/#

Example:
            /sendDetails@name/:Uriel/#phone/:972512345678/#pet/:Dog/#

            The Route is /sendDetails
            The Map is {
                            "name":"Uriel", "phone":"972512345678", "hobby":"Dive"
                        }

*/


#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <any>
#include <typeinfo>
#include <vector>

using namespace std;

#define ERROR_ROUTE -1
#define ERROR_SERIALIZE_PACK -2

#define SUCCESS_SERIALIZE_PACK 1

const string CELL_MIDDLE_VALUE = "/:";
const string CELL_LAST_VALUE = "/#";
const char AT_SIGN_ROUTE = '@';
const char TILTA = '~';

string serialize_cast(const any a);
string serialize_map(const map<string,any>& args);
int serialize_route(const string s, string *rout);
int serialize_str(const string& s, map<string,any> *args);
vector<string> splitByDelimiter(const string& str, const string& delimiter);


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

#endif