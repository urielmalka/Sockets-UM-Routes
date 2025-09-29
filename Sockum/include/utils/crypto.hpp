/**
 * @file crypto.hpp
 * @brief Cryptographic utilities and protocols for Sockum.
 *
 * Provides hex helpers, key derivation, message signing/verification,
 * Double Ratchet for symmetric messaging, and X3DH key agreement,
 * implemented on top of libsodium.
 */
#ifndef CRYPTO_HPP 
#define CRYPTO_HPP

#include <string>
#include <iostream>
#include <sodium.h>
#include <string>
#include <vector>
#include <array>
#include <cstring>
#include <iostream>
#include <iomanip> // for std::hex
#include <sstream>

std::string to_hex(const std::vector<unsigned char>& data);
std::vector<unsigned char> from_hex(const std::string& hex_str);

std::string to_hex_array(const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES>& arr);

std::string to_hex_key(const std::array<unsigned char, crypto_secretbox_KEYBYTES>& key);

static void kdf(const std::array<unsigned char, crypto_secretbox_KEYBYTES>& input,
                std::array<unsigned char, crypto_secretbox_KEYBYTES>& out1,
                std::array<unsigned char, crypto_secretbox_KEYBYTES>& out2);


// Structure to hold a signed message
struct SignedMessage {
    std::string message;
    std::array<unsigned char, crypto_sign_BYTES> signature;
    std::array<unsigned char, crypto_sign_PUBLICKEYBYTES> sender_public_key;
    
    // Serialize for encryption
    std::vector<unsigned char> serialize() const {
        std::vector<unsigned char> result;
        
        // Add message length (4 bytes)
        uint32_t msg_len = message.length();
        result.insert(result.end(), reinterpret_cast<const unsigned char*>(&msg_len), 
                     reinterpret_cast<const unsigned char*>(&msg_len) + sizeof(msg_len));
        
        // Add message
        result.insert(result.end(), message.begin(), message.end());
        
        // Add signature
        result.insert(result.end(), signature.begin(), signature.end());
        
        // Add sender public key
        result.insert(result.end(), sender_public_key.begin(), sender_public_key.end());
        
        return result;
    }
    
    // Deserialize from decrypted data
    static SignedMessage deserialize(const std::vector<unsigned char>& data) {
        SignedMessage msg;
        size_t offset = 0;
        
        // Read message length
        uint32_t msg_len;
        std::memcpy(&msg_len, data.data() + offset, sizeof(msg_len));
        offset += sizeof(msg_len);
        
        // Read message
        msg.message = std::string(data.begin() + offset, data.begin() + offset + msg_len);
        offset += msg_len;
        
        // Read signature
        std::memcpy(msg.signature.data(), data.data() + offset, crypto_sign_BYTES);
        offset += crypto_sign_BYTES;
        
        // Read sender public key
        std::memcpy(msg.sender_public_key.data(), data.data() + offset, crypto_sign_PUBLICKEYBYTES);
        
        return msg;
    }
};


class DoubleRatchet {
    private:
        std::array<unsigned char, crypto_secretbox_KEYBYTES> root_key;
        std::array<unsigned char, crypto_secretbox_KEYBYTES> chain_key;

    public:
        void initialize_as_initiator(const std::array<unsigned char, crypto_generichash_BYTES>& shared_secret);
        void initialize_as_responder(const std::array<unsigned char, crypto_generichash_BYTES>& shared_secret);
        std::vector<unsigned char> encrypt(const std::string& plaintext);
        std::vector<unsigned char> encrypt(const std::vector<unsigned char>& plaintext);
        //std::string decrypt(const std::vector<unsigned char>& ciphertext);
        std::vector<unsigned char> decrypt(const std::vector<unsigned char>& ciphertext);
        std::array<unsigned char, crypto_secretbox_KEYBYTES> get_chain_key() const {
            return chain_key;
        }

};

struct X3DHKeys {
    std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> identity_pub;
    std::array<unsigned char, crypto_kx_SECRETKEYBYTES> identity_priv;

    std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> signed_prekey_pub;
    std::array<unsigned char, crypto_kx_SECRETKEYBYTES> signed_prekey_priv;

    std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> one_time_pub;
    std::array<unsigned char, crypto_kx_SECRETKEYBYTES> one_time_priv;

    // Ed25519 signing keys
    std::array<unsigned char, crypto_sign_PUBLICKEYBYTES> sign_pub;
    std::array<unsigned char, crypto_sign_SECRETKEYBYTES> sign_priv;

    void generate();
};


class X3DH {
    public:
        static void generate_identity_key(X3DHKeys &keys);
        static void generate_prekeys(X3DHKeys &keys);

        // Fixed: Separate functions for Client and Server
        static void derive_shared_secret_client(
            const X3DHKeys &client_keys,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &server_identity_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &server_signed_prekey_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &server_one_time_pub,
            std::array<unsigned char, crypto_generichash_BYTES> &shared_secret
        );

        static void derive_shared_secret_server(
            const X3DHKeys &server_keys,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &client_identity_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &client_signed_prekey_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &client_one_time_pub,
            std::array<unsigned char, crypto_generichash_BYTES> &shared_secret
        );
};


SignedMessage sign_message(const std::string& message, const X3DHKeys& sender_keys);
bool verify_message(const SignedMessage& signed_msg);

#endif