#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "fileConfig.h"
#include "database.h"
#include "crypto.h"


Database* init_db_struct(DBdir* dbdir)
{
    if(dbdir == NULL)
        return NULL;

    Database* database = malloc(sizeof(Database));
    if(database == NULL)
        return NULL;

    database->dbdir = dbdir;

    database->encuser = malloc(sizeof(EncryptUser) * 4);
    if(database->encuser == NULL)
        return NULL;

    database->count = 0;
    database->capasity = 4;

    database->next_id = 1000;

    return database;
}

int db_load(Database* database)
{
    if(database == NULL ||
        database->dbdir == NULL ||
        database->dbdir->dirpath == NULL
    )
        return 1;

    FILE* file = fopen(database->dbdir->currentfilepath, "rb");
    
    if(file == NULL)
        return 1;

    if(fseek(file, 0, SEEK_END) != 0){
        fclose(file);
        return 1;
    }

    long long size = ftell(file);

    if(size < 0){
        fclose(file);
        return 1;
    }

    if(size == 0){
        fclose(file);
        return 0;
    }

    rewind(file);

    
}