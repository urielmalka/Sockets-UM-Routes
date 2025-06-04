/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief Utility functions for Base64 encoding and decoding.
 * 
 * This header defines two standalone functions for encoding and decoding strings using the Base64 algorithm.
 * 
 * Base64 is commonly used to encode binary data as ASCII text, useful for:
 * - Embedding binary data in text-based formats (e.g., JSON, XML)
 * - Sending binary data over media that are designed to handle text
 * 
 * Functions:
 * - std::string base64_encode(const std::string& in): Encodes a binary or ASCII string to Base64 format.
 * - std::string base64_decode(const std::string& in): Decodes a Base64-encoded string back to its original form.
 */


#ifndef BASE64_HPP
#define BASE64_HPP

#include <sstream>
#include <string>
#include <map>
#include <any>
#include <typeinfo>
#include <iomanip>
#include <iostream>
#include <vector>

// Base64 character set
static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

/**
 * @brief Encodes a string into Base64 format.
 * @param in The input string to encode.
 * @return A Base64-encoded representation of the input.
 */

std::string base64_encode(const std::string& in) {
    std::string out;
    int val=0, valb=-6;
    for (uint8_t c : in) {
        val = (val<<8) + c;
        valb += 8;
        while (valb>=0) {
            out.push_back(base64_chars[(val>>valb)&0x3F]);
            valb-=6;
        }
    }
    if (valb>-6) out.push_back(base64_chars[((val<<8)>>(valb+8))&0x3F]);
    while (out.size()%4) out.push_back('=');
    return out;
}

std::string base64_decode(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) {
        T[base64_chars[i]] = i;
    }

    int val = 0, valb = -8;
    for (uint8_t c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

#endif // BASE64_HPP
