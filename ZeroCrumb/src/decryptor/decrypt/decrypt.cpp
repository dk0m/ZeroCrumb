#include "decrypt.h"

BOOL decryptor::initSodium() {
    return sodium_init() != -1;
}

string decryptor::decrypt(PBYTE key, vector<BYTE>& ciphertext) {
    SIZE_T ciphertextSize = ciphertext.size() - 15;

    PBYTE nonce = new BYTE[12];

    std::copy_n(ciphertext.begin() + 3, 12, nonce);

    PBYTE outCiphertext = new BYTE[ciphertextSize];
    std::copy_n(ciphertext.begin() + 15, ciphertextSize, outCiphertext);

    PBYTE decrypted = new BYTE[ciphertextSize - 15];
    ULONGLONG decryptedLen = 0;

    if (crypto_aead_aes256gcm_decrypt(decrypted, &decryptedLen, NULL, outCiphertext, ciphertextSize, NULL, 0, nonce, key) != 0) {
        return defaultResult;
    }

    string decryptedStr((CHAR*)(decrypted), decryptedLen);
    return decryptedStr.substr(32);
}