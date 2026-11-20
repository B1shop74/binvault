#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include <sodium.h>
#include <stdbool.h>

typedef struct{
    EncryptUser* encuser;
    DBdir* dbdir;

    uint64_t count;
    size_t capasity;

    uint64_t next_id;
    long after_bytes;
}Database;

#endif