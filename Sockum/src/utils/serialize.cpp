#include "utils/serialize.hpp"
#include "utils/base64.hpp"

#include <any>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <functional>

using namespace std;

string serialize_cast(const any a) {
    static const unordered_map<type_index, function<string(const any&)>> serializers = {
        {type_index(typeid(string)), [](const any& val) { return any_cast<string>(val); }},
        {type_index(typeid(const char*)), [](const any& val) { return string(any_cast<const char*>(val)); }},
        {type_index(typeid(int)), [](const any& val) { return to_string(any_cast<int>(val)); }},
        {type_index(typeid(double)), [](const any& val) { return to_string(any_cast<double>(val)); }},
        {type_index(typeid(float)), [](const any& val) { return to_string(any_cast<float>(val)); }},
        {type_index(typeid(bool)), [](const any& val) { return any_cast<bool>(val) ? "true" : "false"; }},
        {type_index(typeid(uintmax_t)), [](const any& val) { return to_string(any_cast<uintmax_t>(val)); }},
    };

    auto it = serializers.find(type_index(a.type()));
    if (it != serializers.end()) {
        return it->second(a);
    }
    return "<unsupported>";
}


string serialize_map(const map<string, any>& args) {
    ostringstream buffer;

    if (args.empty()) {
        buffer << TILTA;
        return buffer.str();
    }

    for (const auto& pair : args) {
        buffer << pair.first << CELL_MIDDLE_VALUE
               << (pair.second.has_value() ? base64_encode(serialize_cast(pair.second)) : "")
               << CELL_LAST_VALUE;
    }

    return buffer.str() + TILTA;
}

int serialize_str(const string& s, map<string, any>* args) {
    string tempS, first, second;
    size_t pos = 0;

    tempS = s;
    size_t index = s.find(TILTA);
    if (index != string::npos)
        tempS = s.substr(0, index);

    while (pos < tempS.size()) {
        size_t end = tempS.find(CELL_MIDDLE_VALUE, pos);
        if (end == string::npos) return ERROR_SERIALIZE_PACK;
        first = tempS.substr(pos, end - pos);
        pos = end + CELL_MIDDLE_VALUE.length();

        end = tempS.find(CELL_LAST_VALUE, pos);
        if (end == string::npos) return ERROR_SERIALIZE_PACK;
        second = tempS.substr(pos, end - pos);
        pos = end + CELL_LAST_VALUE.length();

        (*args)[first] = static_cast<any>(base64_decode(second));
    }

    return SUCCESS_SERIALIZE_PACK;
}

int serialize_route(const string s, string* rout) {
    size_t index = s.find(AT_SIGN_ROUTE);
    if (index == string::npos) return ERROR_ROUTE;

    *rout = s.substr(0, index);
    return index + 1;
}

vector<string> splitByDelimiter(const string& str, const string& delimiter) {
    vector<string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != string::npos) {
        if (str.substr(start, end - start).size() > 0)
            result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    if (str.substr(start).size() > 0)
        result.push_back(str.substr(start));

    return result;
}
