#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "crypto.h"


Crypto* crypto_init(){
    Crypto* crypto = calloc(1, sizeof(Crypto));
    return crypto;
}


int crypto_init_salt(Crypto* crypto){
    if(crypto == NULL)
        return 1;

    randombytes_buf(crypto->salt, sizeof crypto->salt);
    crypto->is_initsalt = true;

    return 0;
}

int crypto_init_key(Crypto* crypto, const char* masterpass){
    if(masterpass == NULL || crypto == NULL)
        return 1;

    if(crypto->is_initsalt == false)
        return 1;

    
    if(crypto_pwhash(
        crypto->key,
        sizeof(crypto->key),
        masterpass,
        strlen(masterpass),
        crypto->salt,
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE,
        crypto_pwhash_ALG_ARGON2ID13
    ) != 0)
    {
        sodium_memzero(crypto->key, sizeof(crypto->key));
        return 1;
    }

    crypto->is_initkey = true;
    return 0;
}


int Encrypt(const Crypto* crypto, EncryptField* field, const unsigned char* argv1){
    if (argv1 == NULL ||
        crypto == NULL ||
        !crypto->is_initsalt ||
        !crypto->is_initkey ||
        field == NULL
    ){
        return 1;
    }


    *field = (EncryptField){0};
    randombytes_buf(field->nonce, sizeof(field->nonce));

    field->ciphertext = malloc(
        strlen(argv1)
        + crypto_aead_xchacha20poly1305_ietf_ABYTES
    );

    if(field->ciphertext == NULL)
        return 1;

    field->cptext_len =
    strlen(argv1)
    + crypto_aead_xchacha20poly1305_ietf_ABYTES;


    randombytes_buf(
        field->nonce,
        crypto_aead_xchacha20poly1305_ietf_NPUBBYTES
    );

    if(crypto_aead_xchacha20poly1305_ietf_encrypt(
        field->ciphertext,
        &field->cptext_len,
        argv1,
        strlen(argv1),
        NULL,
        0,
        NULL,
        field->nonce,
        crypto->key
    ) != 0)
    {
        free(field->ciphertext);
        field->ciphertext = NULL;
        field->cptext_len = 0;
        return 1;
    }

    return 0;
}


int Decrypt(const Crypto* crypto, EncryptField* field, DecryptField* defield){
    if (
        crypto == NULL ||
        !crypto->is_initsalt ||
        !crypto->is_initkey ||
        field == NULL ||
        field->ciphertext == NULL ||
        field->cptext_len < crypto_aead_xchacha20poly1305_ietf_ABYTES ||
        defield == NULL
    ){
        return 1;
    }

    if(defield->plaintext != NULL)
        return 1;

    *defield = (DecryptField){0};

    unsigned char* tmp = malloc(
        field->cptext_len
        - crypto_aead_xchacha20poly1305_ietf_ABYTES
        + 1
    );
    if(tmp == NULL)
        return 1;

    unsigned long long tmpl = 0;
    
    if(crypto_aead_xchacha20poly1305_ietf_decrypt
        (
            tmp,
            &tmpl,
            NULL,
            field->ciphertext,
            field->cptext_len,
            NULL,
            0,
            field->nonce,
            crypto->key
        ) != 0)
    {
        free(tmp);
        return 1;
    }

    tmp[tmpl] = '\0';
    defield->plaintext = tmp;

    defield->pltext_len = tmpl;

    return 0;
}


int encryptUserData(const User* user, const Crypto* crypto, EncryptUser* encuser){
    if(user == NULL || crypto == NULL ||
        !crypto->is_initsalt || 
        !crypto->is_initkey ||
        encuser == NULL ||
        user->name == NULL ||
        user->password == NULL ||
        user->about == NULL ||
        user->id == 0
    )
        return 1;


    *encuser = (EncryptUser){0};
    EncryptField field = {0};

    if(Encrypt(crypto, &field, user->name) != 0)
        return 1;
    encuser->name = field;
    field = (EncryptField){0};
    
    if(Encrypt(crypto, &field, user->password) != 0){
        field = (EncryptField){0};
        encuser->name = field;

        return 1;
    }
    encuser->password = field;
    field = (EncryptField){0};

    if(Encrypt(crypto, &field, user->about) != 0){
        field = (EncryptField){0};
        encuser->name = field;
        encuser->password = field;

        return 1;
    }
    encuser->about = field;
    field = (EncryptField){0};

    encuser->id = user->id;

    return 0;
}


int decryptUserData(User* user, const Crypto* crypto, EncryptUser* encuser){
    if(user == NULL || crypto == NULL ||
        !crypto->is_initsalt || 
        !crypto->is_initkey ||
        encuser == NULL ||
        encuser->id == 0 ||
        encuser->id != user->id
    )
        return 1;

    DecryptField field = {0};

    if(Decrypt(crypto, &encuser->name, &field) != 0)
        return 1;
    user->name = field.plaintext;
    user->namelen = field.pltext_len;
    
    field = (DecryptField){0};

    if(Decrypt(crypto, &encuser->password, &field) != 0){
        free(user->name);
        user->name = NULL;
        user->namelen = 0;

        return 1;
    }    
    user->password = field.plaintext;
    user->passwordlen = field.pltext_len;
    
    field = (DecryptField){0};

    if(Decrypt(crypto, &encuser->about, &field) != 0){
        free(user->name);
        user->name = NULL;
        user->namelen = 0;
        free(user->password);
        user->password = NULL;
        user->passwordlen = 0;

        return 1;
    }    
    user->about = field.plaintext;
    user->aboutlen = field.pltext_len;
    
    field = (DecryptField){0};

    return 0;
}