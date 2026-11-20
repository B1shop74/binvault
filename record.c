#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "record.h"


int SpaceError(const char* argv1){
    const char* space = strchr(argv1, ' ');
    if(space != NULL){
        printf("Don't use space!");
        return 1;
    }

    return 0;    
}

void userInfo(const User* user){
    if(user->name == NULL)
        printf("Username:  NULL");
    else
        printf("Name: %s\n", user->name);

    if(user->id == 0)
        printf("User ID:  NULL");
    else
        printf("Id: %" PRId32 "\n", user->id);
}

int setUsername(User* user, const char* argv1){
    if(argv1 == NULL || user == NULL)
        return 1;


    char *tmp = malloc(
        strlen(argv1) 
        + 1
    );
    if (tmp == NULL)
        return 2;

    user->name = tmp;

    user->namelen = strlen(user->name);

    return 0;
}

int setPassword(User* user, const char* argv1){
    if(argv1 == NULL || user == NULL)
        return 1;

    char *tmp = malloc(
        strlen(argv1) 
        + 1
    );
    if (tmp == NULL)
        return 2;
        
    user->password = tmp;

    user->passwordlen = strlen(user->password);

    return 0;
}

int setAbout(User* user, const char* argv1){
    if(argv1 == NULL || user == NULL)
        return 1;

    char *tmp = malloc(
        strlen(argv1) 
        + 1
    );
    if (tmp == NULL)
        return 2;
        
    user->about = tmp;

    user->aboutlen = strlen(user->about);

    return 0;
}   