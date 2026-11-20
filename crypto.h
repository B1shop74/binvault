#ifndef CRYPTO_C
#define CRYPTO_C

#include <stdbool.h>
#include <sodium.h>

#include "record.h"

typedef struct{
    unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
    unsigned char salt[crypto_pwhash_SALTBYTES];

    bool is_initkey;
    bool is_initsalt;
}Crypto;


typedef struct{
    unsigned char* ciphertext;
    unsigned long long cptext_len;

    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
}EncryptField;


typedef struct{
    unsigned char* plaintext;
    unsigned long long pltext_len;
}DecryptField;

typedef struct{
    uint64_t id;

    EncryptField name;
    EncryptField password;
    EncryptField about;
}EncryptUser;


Crypto* crypto_init();

int crypto_init_salt(Crypto* crypto);
int crypto_init_key(Crypto* crypto, const char* masterpass);

int Encrypt(const Crypto* crypto, EncryptField* field, const unsigned char* argv1);
int Decrypt(const Crypto* crypto, EncryptField* field, DecryptField* defield);

int encryptUserData(const User* user, const Crypto* crypto, EncryptUser* encuser);
int decryptUserData(User* user, const Crypto* crypto, EncryptUser* encuser);


#endif