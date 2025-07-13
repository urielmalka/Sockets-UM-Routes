/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief Utility class for managing and assembling chunked network messages.
 * 
 * The ManagePack class is responsible for handling the fragmentation and reassembly of large messages
 * sent over a network. It splits messages into smaller chunks based on a maximum size limit and
 * reassembles them using a message ID and client identifier.
 * 
 * Core functionalities include:
 * - Splitting long messages into chunks with associated message IDs (`chunk_string_for_network`)
 * - Reconstructing original messages from received chunks (`manegePack`, `getPack`)
 * - Tracking message chunks per client ID
 * - Managing active client IDs in the system (`add_cid`, `remove_cid`)
 */


#ifndef MANAGEPACK_HPP
#define MANAGEPACK_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "utils/crypto.hpp"
#include "../external/UColor_C/include/color.h"

using namespace std;


class CryptoPack
{
    private:
        X3DHKeys local_keys;
        std::array<unsigned char, crypto_generichash_BYTES> shared_secret;
        DoubleRatchet send_ratchet, receive_ratchet;
        bool is_server;

    public:
        CryptoPack(bool server_mode = false) : is_server(server_mode) {
            local_keys = X3DHKeys();
            local_keys.generate();
        };
        ~CryptoPack() = default;

        void set_shared_secret(const X3DHKeys& remote_keys) {
            if (is_server) {
                // Server is always "Bob" in X3DH
                X3DH::derive_shared_secret_bob(
                    local_keys,           // Bob's keys (server)
                    remote_keys.identity_pub,
                    remote_keys.signed_prekey_pub,
                    remote_keys.one_time_pub,
                    shared_secret
                );
            } else {
                // Client is always "Alice" in X3DH
                X3DH::derive_shared_secret_alice(
                    local_keys,           // Alice's keys (client)
                    remote_keys.identity_pub,
                    remote_keys.signed_prekey_pub,
                    remote_keys.one_time_pub,
                    shared_secret
                );
            }

            std::cout << "shared_secret: " << to_hex_array(shared_secret) << "\n";
            if (is_server) {
                receive_ratchet.initialize_as_responder(shared_secret);
                send_ratchet.initialize_as_initiator(shared_secret);
            } else {
                send_ratchet.initialize_as_responder(shared_secret);
                receive_ratchet.initialize_as_initiator(shared_secret);
            }
        }

        X3DHKeys& get_x3dh_keys() {
            return local_keys;
        }

        std::array<unsigned char, crypto_generichash_BYTES>& get_shared_secret() {
            return shared_secret;
        }

        DoubleRatchet& get_send_ratchet() {
            return send_ratchet;
        }

        DoubleRatchet& get_receive_ratchet() {
            return receive_ratchet;
        }

};


class ManagePack
{
    private:
        map<string, map<int,string>> network_packs;
        int maxPack = 32768 - 100; // 32KB - 100 bytes for encryption overhead

        map< string, CryptoPack> crypto_packs;


    public:
        ManagePack();
        ManagePack(int maxP);
        ~ManagePack();

        bool manegePack(const string& cid, const string& pack, int& message_id);
        string getPack(const string& cid, int messageId);
        
        /**
         * @brief Splits a string into chunks suitable for network transmission.
         * @param pack The full message to be split.
         * @param message_id The unique identifier for this message, used to track chunks.
         * @return A vector of string chunks, each of size <= maxPack.
         */
        vector<string> chunk_string_for_network(const string& pack, int message_id);


        bool add_cid(const string& cid);
        bool remove_cid(const string& cid);

        void add_crypto_pack(const string& cid);
        CryptoPack& get_crypto_pack(const string& cid) {
            return crypto_packs[cid];
        }

        void set_shared_secret(const string& cid, const X3DHKeys& keys);

};

#endif