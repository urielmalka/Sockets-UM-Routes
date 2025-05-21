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
string serialize_map(const map<string, any>& args);
int serialize_route(const string s, string* rout);
int serialize_str(const string& s, map<string, any>* args);
vector<string> splitByDelimiter(const string& str, const string& delimiter);

#endif // SERIALIZE_HPP
