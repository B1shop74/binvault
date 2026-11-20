#ifndef RECORD_H
#define RECORD_H

#include <stdint.h>

typedef struct{
    uint64_t id;

    char* name;
    unsigned long long namelen;

    char* password;
    unsigned long long passwordlen;

    char* about;
    unsigned long long aboutlen;
}User;

int SpaceError(const char* argv1);
void userInfo(const User* user);

int setUsername(User* user, const char* argv1);
int setPassword(User* user, const char* argv1);
int setAbout(User* user, const char* argv1);

#endif