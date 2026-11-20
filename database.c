#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sodium.h>

#include "fileConfig.h"
#include "database.h"
#include "crypto.h"


typedef struct {
    EncryptUser *encuser;
    DBdir *dbdir;

    uint64_t count;
    size_t capasity;

    unsigned long long next_id;
    long after_bytes;
} Database;


Database *init_db_struct (DBdir *dbdir)
{
    if (dbdir == NULL ||
        dbdir->dirpath == NULL ||
        dbdir->currentfilepath == NULL)
        return NULL;

    Database *database = malloc(sizeof(*database));

    if (database == NULL)
        return NULL;

    database->dbdir = dbdir;

    database->encuser = calloc(4, sizeof(*database->encuser));

    if (database->encuser == NULL) {
        free(database);
        return NULL;
    }

    database->count = 0;
    database->capasity = 4;
    database->next_id = 1000;
    database->after_bytes = 16;

    return database;
}


static void free_users(EncryptUser *users, uint64_t count)
{
    if (users == NULL)
        return;

    for (uint64_t i = 0; i < count; i++) {
        free(users[i].name.ciphertext);
        free(users[i].password.ciphertext);
        free(users[i].about.ciphertext);
    }

    free(users);
}


static int if_errors (Database* database)
{
    if (database == NULL ||
        database->dbdir == NULL ||
        database->dbdir->currentfilepath == NULL)
        return 1;
    else
        return 0;
}


int add_file(Database *database)
{
    if (if_errors(database) == 1)
        return 1;

    FILE *file = fopen(database->dbdir->currentfilepath, "rb");

    if (file != NULL) {
        fclose(file);
        return 1;
    }

    file = fopen(database->dbdir->currentfilepath, "wb");

    if (file == NULL)
        return 1;

    const unsigned char magic[4] = {'B', 'V', 'L', 'T'};
    uint32_t version = 1;

    if (fwrite(magic, 1, sizeof(magic), file) != sizeof(magic) ||
        fwrite(&version, sizeof(version), 1, file) != 1 ||
        fwrite(&database->count, sizeof(database->count), 1, file) != 1) {
        fclose(file);
        return 1;
    }

    long pos = ftell(file);

    if (pos < 0) {
        fclose(file);
        return 1;
    }

    if (fclose(file) != 0)
        return 1;

    database->after_bytes = pos;
    return 0;
}


int db_read_header(FILE *file, uint64_t *count)
{
    unsigned char magic[4];
    uint32_t version;

    if (file == NULL || count == NULL)
        return 1;

    if (fseek(file, 0, SEEK_SET) != 0)
        return 1;

    if (fread(magic, 1, sizeof(magic), file) != sizeof(magic))
        return 1;

    if (memcmp(magic, "BVLT", sizeof(magic)) != 0)
        return 1;

    if (fread(&version, sizeof(version), 1, file) != 1)
        return 1;

    if (version != 1)
        return 1;

    if (fread(count, sizeof(*count), 1, file) != 1)
        return 1;

    return 0;
}


int db_load(Database *database)
{
    if (if_errors(database) == 1)
        return 1;


    FILE *file = fopen(database->dbdir->currentfilepath, "rb");

    if (file == NULL)
        return 1;

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return 1;
    }

    long file_size = ftell(file);

    if (file_size < 16) {
        fclose(file);
        return 1;
    }

    uint64_t count;

    if (db_read_header(file, &count) != 0) {
        fclose(file);
        return 1;
    }

    if (count > SIZE_MAX / sizeof(EncryptUser)) {
        fclose(file);
        return 1;
    }

    
    const uint64_t min_record_size =
        sizeof(uint64_t) +
        3 * (sizeof(unsigned long long) +
             crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);

    if (count > (uint64_t)(file_size - 16) / min_record_size) {
        fclose(file);
        return 1;
    }

    size_t capacity = count > 4 ? (size_t)count : 4;

    EncryptUser *tmpl = calloc(capacity, sizeof(*tmpl));

    if (tmpl == NULL) {
        fclose(file);
        return 1;
    }

    uint64_t max_id = 999;

    for (uint64_t i = 0; i < count; i++) {
        if (fread(&tmpl[i].id, sizeof(tmpl[i].id), 1, file) != 1) {
            free_users(tmpl, count);
            fclose(file);
            return 1;
        }

        if (tmpl[i].id > max_id)
            max_id = tmpl[i].id;

        EncryptField *fields[3] = {
            &tmpl[i].name,
            &tmpl[i].password,
            &tmpl[i].about
        };

        for (int j = 0; j < 3; j++) {
            unsigned long long len;

            if (fread(&len, sizeof(len), 1, file) != 1) {
                free_users(tmpl, count);
                fclose(file);
                return 1;
            }

            long pos = ftell(file);

            if (pos < 0 || pos > file_size ||
                len > (unsigned long long)(file_size - pos) ||
                (unsigned long long)(file_size - pos) - len <
                    crypto_aead_xchacha20poly1305_ietf_NPUBBYTES) {
                free_users(tmpl, count);
                fclose(file);
                return 1;
            }

            if (len > 0) {
                fields[j]->ciphertext = malloc((size_t)len);

                if (fields[j]->ciphertext == NULL) {
                    free_users(tmpl, count);
                    fclose(file);
                    return 1;
                }

                if (fread(fields[j]->ciphertext, 1, (size_t)len, file)
                    != (size_t)len) {
                    free_users(tmpl, count);
                    fclose(file);
                    return 1;
                }
            }

            fields[j]->cptext_len = len;

            if (fread(fields[j]->nonce, 1, sizeof(fields[j]->nonce), file)
                != sizeof(fields[j]->nonce)) {
                free_users(tmpl, count);
                fclose(file);
                return 1;
            }
        }
    }


    if (ftell(file) != file_size) {
        free_users(tmpl, count);
        fclose(file);
        return 1;
    }

    if (fclose(file) != 0) {
        free_users(tmpl, count);
        return 1;
    }

    free_users(database->encuser, database->count);

    database->encuser = tmpl;
    database->count = count;
    database->capasity = capacity;
    database->after_bytes = 16;

    if (max_id == UINT64_MAX) {
        database->next_id = 0;
    } else {
        database->next_id = max_id + 1;

        if (database->next_id < 1000)
            database->next_id = 1000;
    }

    return 0;
}


int db_save(const Database* database)
{
    if (if_errors(database) == 1)
        return 1;


    FILE* file = fopen(database->dbdir->currentfilepath, "wb");
    if (file == NULL) 
        return 1;


    const unsigned char magict[4] = {'B', 'V', 'L', 'T'};
    uint32_t version = 1;

    if (fwrite(magict, 1, 4, file) != 4) {
        fclose(file);
        return 1;
    }

    if (fwrite(&version, sizeof(version), 1, file) != 1) {
        fclose(file);
        return 1;
    }

    if (database->count > 0) {
        if (fwrite(database->count, sizeof(database->count), 1, file) != 1) {
            fclose(file);
            return 1;
        }

        for (uint64_t i = 0; i < database->count; i++) {
            if (fwrite(database->encuser->id, sizeof(database->encuser->id), 1, file) != 1) {
                fclose(file);
                return 1;
            }
            for (uint64_t j = 0; j < 4; j++) {

            }
        }
    }
    else {
        if (fwrite(database->count, sizeof(database->count), 1, file) != 1) {
            fclose(file);
            return 1;
        }
        fclose(file);
        return 0;
    }
}