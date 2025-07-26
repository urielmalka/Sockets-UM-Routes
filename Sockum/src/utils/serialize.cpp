#include "utils/serialize.hpp"
#include "utils/base64.hpp"

#include <any>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;

string serialize_cast(const any);  // forward

template<typename T>
string serialize_vector(const vector<T>& vec) {
    string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if constexpr (is_same<T, string>::value)
            result += "\"" + vec[i] + "\"";
        else if constexpr (is_same<T, const char*>::value)
            result += "\"" + string(vec[i]) + "\"";
        else if constexpr (is_same<T, bool>::value)
            result += vec[i] ? "true" : "false";
        else
            result += to_string(vec[i]);
        
        if (i != vec.size() - 1)
            result += ", ";
    }
    result += "]";
    return result;
}

template<typename T>
string serialize_map(const map<string, T>& m) {
    string result = "{";
    size_t count = 0;
    for (const auto& [key, value] : m) {
        result += "\"" + key + "\": ";
        if constexpr (is_same<T, string>::value)
            result += "\"" + value + "\"";
        else if constexpr (is_same<T, const char*>::value)
            result += "\"" + string(value) + "\"";
        else if constexpr (is_same<T, bool>::value)
            result += value ? "true" : "false";
        else
            result += to_string(value);

        if (++count != m.size())
            result += ", ";
    }
    result += "}";
    return result;
}


string serialize_vector_any(const vector<any>& vec) {
    string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += "\"" + serialize_cast(vec[i]) + "\"";
        if (i != vec.size() - 1)
            result += ", ";
    }
    result += "]";
    return result;
}

string serialize_map_any(const map<string, any>& m) {
    string result = "{";
    size_t count = 0;
    for (const auto& [key, val] : m) {
        result += "\"" + key + "\": ";

        string value_str = serialize_cast(val);
        if (val.type() == typeid(string) || val.type() == typeid(const char*))
            result += "\"" + value_str + "\"";
        else
            result += value_str;

        if (++count != m.size())
            result += ", ";
    }
    result += "}";
    return result;
}

string serialize_vector_of_map_any(const vector<map<string, any>>& vec) {
    string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += serialize_map_any(vec[i]);
        if (i != vec.size() - 1)
            result += ", ";
    }
    result += "]";
    return result;
}


string serialize_cast(const any a) {
      static const unordered_map<type_index, function<string(const any&)>> serializers = {
        {type_index(typeid(string)), [](const any& val) { return any_cast<string>(val); }},
        {type_index(typeid(const char*)), [](const any& val) { return string(any_cast<const char*>(val)); }},
        {type_index(typeid(int)), [](const any& val) { return to_string(any_cast<int>(val)); }},
        {type_index(typeid(double)), [](const any& val) { return to_string(any_cast<double>(val)); }},
        {type_index(typeid(float)), [](const any& val) { return to_string(any_cast<float>(val)); }},
        {type_index(typeid(bool)), [](const any& val) { return any_cast<bool>(val) ? "true" : "false"; }},
        {type_index(typeid(uintmax_t)), [](const any& val) { return to_string(any_cast<uintmax_t>(val)); }},
        {type_index(typeid(vector<any>)), [](const any& val) {
            return serialize_vector_any(any_cast<vector<any>>(val));
        }},
        {type_index(typeid(vector<string>)), [](const any& val) {
            return serialize_vector(any_cast<vector<string>>(val));
        }},
        {type_index(typeid(vector<int>)), [](const any& val) {
            return serialize_vector(any_cast<vector<int>>(val));
        }},
        {type_index(typeid(vector<double>)), [](const any& val) {
            return serialize_vector(any_cast<vector<double>>(val));
        }},
        {type_index(typeid(vector<bool>)), [](const any& val) {
            return serialize_vector(any_cast<vector<bool>>(val));
        }},
        {type_index(typeid(vector<float>)), [](const any& val) {
            return serialize_vector(any_cast<vector<float>>(val));
        }},
        {type_index(typeid(map<string, string>)), [](const any& val) {
        return serialize_map(any_cast<map<string, string>>(val));
        }},
        {type_index(typeid(map<string, int>)), [](const any& val) {
            return serialize_map(any_cast<map<string, int>>(val));
        }},
        {type_index(typeid(map<string, double>)), [](const any& val) {
            return serialize_map(any_cast<map<string, double>>(val));
        }},
        {type_index(typeid(map<string, bool>)), [](const any& val) {
            return serialize_map(any_cast<map<string, bool>>(val));
        }},
        {type_index(typeid(map<string, float>)), [](const any& val) {
            return serialize_map(any_cast<map<string, float>>(val));
        }},
        {type_index(typeid(vector<map<string, any>>)), [](const any& val) {
            return serialize_vector_of_map_any(any_cast<vector<map<string, any>>>(val));
        }},
        {type_index(typeid(map<string, any>)), [](const any& val) {
            return serialize_map_any(any_cast<map<string, any>>(val));
        }},


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


template <typename T>
vector<any> to_any_vector(const vector<T>& vec) {
    vector<any> result;
    result.reserve(vec.size());
    for (const auto& item : vec)
        result.emplace_back(item);
    return result;
}

pair<type_index, vector<any>> unpack_vector_as(const any& a) {
    static const unordered_map<type_index, function<vector<any>(const any&)>> unpackers = {
        { type_index(typeid(vector<int>)), [](const any& val) {
            return to_any_vector(any_cast<const vector<int>&>(val));
        }},
        { type_index(typeid(vector<string>)), [](const any& val) {
            return to_any_vector(any_cast<const vector<string>&>(val));
        }},
        { type_index(typeid(vector<double>)), [](const any& val) {
            return to_any_vector(any_cast<const vector<double>&>(val));
        }},
        { type_index(typeid(vector<float>)), [](const any& val) {
            return to_any_vector(any_cast<const vector<float>&>(val));
        }},
        { type_index(typeid(vector<bool>)), [](const any& val) {
            return to_any_vector(any_cast<const vector<bool>&>(val));
        }},
        { type_index(typeid(vector<any>)), [](const any& val) {
            return any_cast<const vector<any>&>(val);
        }}
    };

    type_index ti = type_index(a.type());
    auto it = unpackers.find(ti);
    if (it != unpackers.end()) {
        return {ti, it->second(a)};
    } else {
        return {type_index(typeid(void)), {}}; 
    }
}

template <typename T>
vector<T> unpack_vector(const any& a) {
    if (a.type() == typeid(vector<T>)) {
        return any_cast<const vector<T>&>(a);
    }

    if (a.type() == typeid(string)) {
        const string& str = any_cast<const string&>(a);
        if (!str.empty() && str.front() == '[') {
            try {
                json j = json::parse(str);

                if constexpr (is_same<T, map<string, any>>::value) {
                    vector<map<string, any>> result;
                    for (const auto& obj : j) {
                        map<string, any> entry;
                        for (const auto& [key, val] : obj.items()) {
                            if (val.is_string())
                                entry[key] = val.template get<std::string>();
                            else if (val.is_number_integer())
                                entry[key] = val.template get<int>();
                            else if (val.is_number_float())
                                entry[key] = val.template get<double>();
                            else if (val.is_boolean())
                                entry[key] = val.template get<bool>();
                            else if (val.is_array()) {
                                std::vector<std::any> vec;
                                for (const auto& x : val) {
                                    if (x.is_string())
                                        vec.push_back(x.template get<std::string>());
                                    else if (x.is_number_integer())
                                        vec.push_back(x.template get<int>());
                                    else if (x.is_number_float())
                                        vec.push_back(x.template get<double>());
                                    else if (x.is_boolean())
                                        vec.push_back(x.template get<bool>());
                                }
                                entry[key] = vec;
                            }
                        }
                        result.push_back(entry);
                    }
                    return result;
                } else {
                    return j.get<vector<T>>();  
                }
            } catch (const std::exception& e) {
                throw runtime_error(string("Failed to parse JSON: ") + e.what());
            }
        }
    }

    auto [type, v] = unpack_vector_as(a);
    if (type == type_index(typeid(vector<T>))) {
        vector<T> result;
        for (const auto& item : v)
            result.push_back(any_cast<T>(item));
        return result;
    }

    throw runtime_error("Cannot unpack to expected vector<T>");
}


template std::vector<std::string> unpack_vector<std::string>(const std::any&);
template std::vector<int> unpack_vector<int>(const std::any&);
template std::vector<double> unpack_vector<double>(const std::any&);
template std::vector<float> unpack_vector<float>(const std::any&);
template std::vector<bool> unpack_vector<bool>(const std::any&);
template std::vector<std::map<std::string, std::any>> unpack_vector<std::map<std::string, std::any>>(const std::any&);


template <typename T>
map<string, T> unpack_map(const any& a) {

    if (a.type() == typeid(map<string, T>)) {
        return any_cast<const map<string, T>&>(a);
    }

    if (a.type() == typeid(string)) {
        const string& str = any_cast<const string&>(a);
        if (!str.empty() && str.front() == '{') {
            try {
                json j = json::parse(str);
                return j.get<map<string, T>>();
            } catch (const std::exception& e) {
                throw runtime_error(string("Failed to parse JSON map: ") + e.what());
            }
        }
    }

    throw runtime_error("Cannot unpack to expected map<string, T>");
}

template map<string, string> unpack_map<string>(const any&);
template map<string, int> unpack_map<int>(const any&);
template map<string, double> unpack_map<double>(const any&);
template map<string, float> unpack_map<float>(const any&);
template map<string, bool> unpack_map<bool>(const any&);    
