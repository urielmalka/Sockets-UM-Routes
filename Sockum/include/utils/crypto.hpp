#ifndef CRYPTO_HPP 
#define CRYPTO_HPP

#include <string>
#include <iostream>


using namespace std;

string encrypt(const string& e);
string decrypt(const string& d);


string encryot(const string& e){ return e; };
string decrypt(const string& d){ return d; };

#endif