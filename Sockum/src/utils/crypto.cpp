#include "crypto.hpp"

std::string to_hex(const std::vector<unsigned char>& data) {
    std::ostringstream oss;
    for (auto byte : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return oss.str();
}

std::vector<unsigned char> from_hex(const std::string& hex_str) {
    std::vector<unsigned char> result;

    if (hex_str.length() % 2 != 0) {
        throw std::invalid_argument("Hex string must have even length");
    }

    for (size_t i = 0; i < hex_str.length(); i += 2) {
        std::string byteString = hex_str.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteString, nullptr, 16));
        result.push_back(byte);
    }

    return result;
}


std::string to_hex_array(const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES>& arr) {
    return to_hex(std::vector<unsigned char>(arr.begin(), arr.end()));
}

auto to_hex_array32 = [](const auto& arr) {
    return to_hex(std::vector<unsigned char>(arr.begin(), arr.end()));
};

std::string to_hex_key(const std::array<unsigned char, crypto_secretbox_KEYBYTES>& key) {
    return to_hex(std::vector<unsigned char>(key.begin(), key.end()));
}

static void kdf(const std::array<unsigned char, crypto_secretbox_KEYBYTES>& input,
                std::array<unsigned char, crypto_secretbox_KEYBYTES>& out1,
                std::array<unsigned char, crypto_secretbox_KEYBYTES>& out2) {
    unsigned char buffer[crypto_secretbox_KEYBYTES * 2];
    crypto_generichash(buffer, sizeof(buffer), input.data(), input.size(), nullptr, 0);
    std::memcpy(out1.data(), buffer, crypto_secretbox_KEYBYTES);
    std::memcpy(out2.data(), buffer + crypto_secretbox_KEYBYTES, crypto_secretbox_KEYBYTES);
}

void DoubleRatchet::initialize_as_initiator(const std::array<unsigned char, crypto_generichash_BYTES>& shared_secret) {
    std::memcpy(root_key.data(), shared_secret.data(), crypto_secretbox_KEYBYTES);
    kdf(root_key, root_key, chain_key);
}

void DoubleRatchet::initialize_as_responder(const std::array<unsigned char, crypto_generichash_BYTES>& shared_secret) {
    std::memcpy(root_key.data(), shared_secret.data(), crypto_secretbox_KEYBYTES);
    kdf(root_key, root_key, chain_key);
}

std::vector<unsigned char> DoubleRatchet::encrypt(const std::string& plaintext) {
    std::array<unsigned char, crypto_secretbox_KEYBYTES> message_key;
    kdf(chain_key, chain_key, message_key);

    std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    std::vector<unsigned char> ciphertext(plaintext.size() + crypto_secretbox_MACBYTES);
    crypto_secretbox_easy(ciphertext.data(), (const unsigned char*)plaintext.data(), plaintext.size(), nonce.data(), message_key.data());

    ciphertext.insert(ciphertext.begin(), nonce.begin(), nonce.end());
    return ciphertext;
}

std::vector<unsigned char> DoubleRatchet::encrypt(const std::vector<unsigned char>& plaintext) {
    std::array<unsigned char, crypto_secretbox_KEYBYTES> message_key;
    kdf(chain_key, chain_key, message_key);

    std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    std::vector<unsigned char> ciphertext(plaintext.size() + crypto_secretbox_MACBYTES);
    crypto_secretbox_easy(ciphertext.data(), plaintext.data(), plaintext.size(), nonce.data(), message_key.data());

    ciphertext.insert(ciphertext.begin(), nonce.begin(), nonce.end());
    return ciphertext;
}

std::vector<unsigned char> DoubleRatchet::decrypt(const std::vector<unsigned char>& ciphertext) {
    std::array<unsigned char, crypto_secretbox_KEYBYTES> message_key;
    kdf(chain_key, chain_key, message_key);

    std::vector<unsigned char> nonce(ciphertext.begin(), ciphertext.begin() + crypto_secretbox_NONCEBYTES);
    std::vector<unsigned char> enc(ciphertext.begin() + crypto_secretbox_NONCEBYTES, ciphertext.end());

    std::vector<unsigned char> decrypted(enc.size() - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(decrypted.data(), enc.data(), enc.size(), nonce.data(), message_key.data()) != 0) {
        return {}; // Return empty vector on failure
    }
    return decrypted;
}


void X3DHKeys::generate() {
    crypto_kx_keypair(identity_pub.data(), identity_priv.data());
    crypto_kx_keypair(signed_prekey_pub.data(), signed_prekey_priv.data());
    crypto_kx_keypair(one_time_pub.data(), one_time_priv.data());
    crypto_sign_keypair(sign_pub.data(), sign_priv.data());
}

void X3DH::generate_identity_key(X3DHKeys &keys) {
    crypto_kx_keypair(keys.identity_pub.data(), keys.identity_priv.data());
}

void X3DH::generate_prekeys(X3DHKeys &keys) {
    crypto_kx_keypair(keys.signed_prekey_pub.data(), keys.signed_prekey_priv.data());
    crypto_kx_keypair(keys.one_time_pub.data(), keys.one_time_priv.data());
}

void X3DH::derive_shared_secret_client(
    const X3DHKeys &client_keys,
    const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &server_identity_pub,
    const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &server_signed_prekey_pub,
    const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &server_one_time_pub,
    std::array<unsigned char, crypto_generichash_BYTES> &shared_secret
) {
    std::array<unsigned char, crypto_scalarmult_BYTES> dh1, dh2, dh3, dh4;

    // Client's perspective: DH calculations
    crypto_scalarmult(dh1.data(), client_keys.identity_priv.data(), server_signed_prekey_pub.data());
    crypto_scalarmult(dh2.data(), client_keys.signed_prekey_priv.data(), server_identity_pub.data());
    crypto_scalarmult(dh3.data(), client_keys.signed_prekey_priv.data(), server_signed_prekey_pub.data());
    crypto_scalarmult(dh4.data(), client_keys.one_time_priv.data(), server_signed_prekey_pub.data());

    unsigned char concat[crypto_scalarmult_BYTES * 4];
    memcpy(concat, dh1.data(), 32);
    memcpy(concat + 32, dh2.data(), 32);
    memcpy(concat + 64, dh3.data(), 32);
    memcpy(concat + 96, dh4.data(), 32);

    crypto_generichash(shared_secret.data(), shared_secret.size(), concat, sizeof(concat), nullptr, 0);
}

void X3DH::derive_shared_secret_server(
    const X3DHKeys &server_keys,
    const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &client_identity_pub,
    const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &client_signed_prekey_pub,
    const std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> &client_one_time_pub,
    std::array<unsigned char, crypto_generichash_BYTES> &shared_secret
) {
    std::array<unsigned char, crypto_scalarmult_BYTES> dh1, dh2, dh3, dh4;

    // Server's perspective: DH calculations (mirror of Client's)
    crypto_scalarmult(dh1.data(), server_keys.signed_prekey_priv.data(), client_identity_pub.data());
    crypto_scalarmult(dh2.data(), server_keys.identity_priv.data(), client_signed_prekey_pub.data());
    crypto_scalarmult(dh3.data(), server_keys.signed_prekey_priv.data(), client_signed_prekey_pub.data());
    crypto_scalarmult(dh4.data(), server_keys.signed_prekey_priv.data(), client_one_time_pub.data());

    unsigned char concat[crypto_scalarmult_BYTES * 4];
    memcpy(concat, dh1.data(), 32);
    memcpy(concat + 32, dh2.data(), 32);
    memcpy(concat + 64, dh3.data(), 32);
    memcpy(concat + 96, dh4.data(), 32);

    crypto_generichash(shared_secret.data(), shared_secret.size(), concat, sizeof(concat), nullptr, 0);
}



// Helper functions for signing and verifying
SignedMessage sign_message(const std::string& message, const X3DHKeys& sender_keys) {
    SignedMessage signed_msg;
    signed_msg.message = message;
    signed_msg.sender_public_key = sender_keys.sign_pub;

    
    
    crypto_sign_detached(signed_msg.signature.data(),
                        nullptr,
                        reinterpret_cast<const unsigned char*>(message.data()),
                        message.size(),
                        sender_keys.sign_priv.data());

    return signed_msg;
}


bool verify_message(const SignedMessage& signed_msg) {
    return crypto_sign_verify_detached(signed_msg.signature.data(),
                                      reinterpret_cast<const unsigned char*>(signed_msg.message.data()),
                                      signed_msg.message.size(),
                                      signed_msg.sender_public_key.data()) == 0;
}