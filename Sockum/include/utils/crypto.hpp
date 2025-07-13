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


class DoubleRatchet {
    private:
        std::array<unsigned char, crypto_secretbox_KEYBYTES> root_key;
        std::array<unsigned char, crypto_secretbox_KEYBYTES> chain_key;

    public:
        void initialize_as_initiator(const std::array<unsigned char, crypto_generichash_BYTES>& shared_secret);
        void initialize_as_responder(const std::array<unsigned char, crypto_generichash_BYTES>& shared_secret);
        std::vector<unsigned char> encrypt(const std::string& plaintext);
        std::string decrypt(const std::vector<unsigned char>& ciphertext);
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

    void generate();
};


class X3DH {
    public:
        static void generate_identity_key(X3DHKeys &keys);
        static void generate_prekeys(X3DHKeys &keys);

        // Fixed: Separate functions for Alice and Bob
        static void derive_shared_secret_alice(
            const X3DHKeys &alice_keys,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &bob_identity_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &bob_signed_prekey_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &bob_one_time_pub,
            std::array<unsigned char, crypto_generichash_BYTES> &shared_secret
        );

        static void derive_shared_secret_bob(
            const X3DHKeys &bob_keys,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &alice_identity_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &alice_signed_prekey_pub,
            const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &alice_one_time_pub,
            std::array<unsigned char, crypto_generichash_BYTES> &shared_secret
        );
};

#endif