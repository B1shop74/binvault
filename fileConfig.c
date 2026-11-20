#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#include "fileConfig.h"


DBdir* set_db_dir(const char* argv1)
{
    if (argv1 == NULL)
        return NULL;

    DBdir* dbdir = calloc(1, sizeof *dbdir);

    if (dbdir == NULL)
        return NULL;

    dbdir->dirpath = malloc(strlen(argv1) + 1);

    if (dbdir->dirpath == NULL) {
        free(dbdir);
        return NULL;
    }

    strcpy(dbdir->dirpath, argv1);

    dbdir->is_dirSct = true;

    return dbdir;
}


int change_db_dir(DBdir* dbdir, const char* argv1){
    if(dbdir == NULL || argv1 == NULL)
        return 1;

    char* tmp = realloc(
        dbdir->dirpath, 
        strlen(argv1) 
        + 1
    );

    if (tmp == NULL)
        return 1;

    dbdir->dirpath = tmp;

    free(dbdir->currentfilename);
    free(dbdir->currentfilepath);

    for (uint32_t i = 0; i < dbdir->count; i++)
        free(dbdir->filesnamelist[i]);

    free(dbdir->filesnamelist);
    
    dbdir->currentfilename = NULL;
    dbdir->currentfilepath = NULL;
    dbdir->filesnamelist = NULL;
    dbdir->count = 0;
    dbdir->is_dirSct = false;

    return 0;
}


int set_db_file(DBdir* dbdir, const char** argv1)
{
    if (dbdir == NULL || argv1 == NULL || argv1[0] == NULL)
        return 1;

    dbdir->index = dbdir->count;

    uint32_t new_count = 0;

    for (int i = 0; argv1[i] != NULL; i++)
        new_count++;

    dbdir->count += new_count;

    char** tmp = realloc(
        dbdir->filesnamelist,
        dbdir->count * sizeof *dbdir->filesnamelist
    );

    if (tmp == NULL)
        return 1;

    dbdir->filesnamelist = tmp;

    for (uint32_t i = dbdir->index, j = 0;
         argv1[j] != NULL;
         i++, j++)
    {
        dbdir->filesnamelist[i] = malloc(strlen(argv1[j]) + 1);

        if (dbdir->filesnamelist[i] == NULL)
            return 1;

        strcpy(dbdir->filesnamelist[i], argv1[j]);
    }

    dbdir->is_listVlb = true;

    if (dbdir->index == 0) {
        dbdir->currentfilename = malloc(strlen(argv1[0]) + 1);

        if (dbdir->currentfilename == NULL)
            return 1;

        strcpy(dbdir->currentfilename, argv1[0]);

        size_t path_len = strlen(dbdir->dirpath);
        size_t name_len = strlen(argv1[0]);

        dbdir->currentfilepath = malloc(
            path_len + 1 + name_len + 1
        );

        if (dbdir->currentfilepath == NULL)
            return 1;

        sprintf(
            dbdir->currentfilepath,
            "%s/%s",
            dbdir->dirpath,
            argv1[0]
        );
    }

    return 0;
}


int set_current_file(DBdir* dbdir, const char* argv1)
{
    if (dbdir == NULL || argv1 == NULL)
        return 1;

    int found = -1;

    for (uint32_t i = 0; i < dbdir->count; i++) {
        if (strcmp(dbdir->filesnamelist[i], argv1) == 0) {
            found = (int)i;
            break;
        }
    }

    if (found == -1)
        return 1;

    char* name_tmp = realloc(
        dbdir->currentfilename,
        strlen(argv1) + 1
    );

    if (name_tmp == NULL)
        return 1;

    dbdir->currentfilename = name_tmp;

    strcpy(dbdir->currentfilename, argv1);

    size_t path_len = strlen(dbdir->dirpath);
    size_t name_len = strlen(argv1);

    char* path_tmp = realloc(
        dbdir->currentfilepath,
        path_len + 1 + name_len + 1
    );

    if (path_tmp == NULL)
        return 1;

    dbdir->currentfilepath = path_tmp;

    sprintf(
        dbdir->currentfilepath,
        "%s/%s",
        dbdir->dirpath,
        argv1
    );

    return 0;
}


int rename_db_file(
    DBdir* dbdir,
    const char* argv1old,
    const char* argv1new
)
{
    if (dbdir == NULL || argv1old == NULL || argv1new == NULL)
        return 1;

    int found = -1;

    for (uint32_t i = 0; i < dbdir->count; i++) {
        if (strcmp(dbdir->filesnamelist[i], argv1old) == 0) {
            found = (int)i;
            break;
        }
    }

    if (found == -1)
        return 1;

    size_t dir_len = strlen(dbdir->dirpath);
    size_t old_len = strlen(argv1old);
    size_t new_len = strlen(argv1new);

    char* old_path = malloc(dir_len + 1 + old_len + 1);

    if (old_path == NULL)
        return 1;

    char* new_path = malloc(dir_len + 1 + new_len + 1);

    if (new_path == NULL) {
        free(old_path);
        return 1;
    }

    sprintf(
        old_path,
        "%s/%s",
        dbdir->dirpath,
        argv1old
    );

    sprintf(
        new_path,
        "%s/%s",
        dbdir->dirpath,
        argv1new
    );

    if (rename(old_path, new_path) != 0) {
        perror("rename");

        free(old_path);
        free(new_path);

        return 1;
    }

    free(old_path);
    free(new_path);

    char* name_tmp = realloc(
        dbdir->filesnamelist[found],
        new_len + 1
    );

    if (name_tmp == NULL)
        return 1;

    dbdir->filesnamelist[found] = name_tmp;

    strcpy(
        dbdir->filesnamelist[found],
        argv1new
    );

    if (dbdir->currentfilename != NULL &&
        strcmp(dbdir->currentfilename, argv1old) == 0)
    {
        char* current_name_tmp = realloc(
            dbdir->currentfilename,
            new_len + 1
        );

        if (current_name_tmp == NULL)
            return 1;

        dbdir->currentfilename = current_name_tmp;

        strcpy(
            dbdir->currentfilename,
            argv1new
        );

        size_t current_path_len = strlen(dbdir->dirpath);

        char* current_path_tmp = realloc(
            dbdir->currentfilepath,
            current_path_len + 1 + new_len + 1
        );

        if (current_path_tmp == NULL)
            return 1;

        dbdir->currentfilepath = current_path_tmp;

        sprintf(
            dbdir->currentfilepath,
            "%s/%s",
            dbdir->dirpath,
            argv1new
        );
    }

    return 0;
}

int remove_db_file(DBdir* dbdir, const char* argv1){
    if(dbdir == NULL || argv1 == NULL)
        return 0;


    char* tmppath = malloc(strlen(dbdir->dirpath) 
                + 1
                + strlen(argv1)
                + 1
            );

            sprintf(
                tmppath, 
                "%s/%s", 
                dbdir->dirpath, 
                argv1
            );

            if (remove(tmppath) != 0) {
                perror("remove");
                free(tmppath);
                return 0;
            }
            else{
                remove(tmppath);
                free(tmppath);
            }
            

    for(int i = 0; i < dbdir->count; i++){
        if(strcmp(dbdir->filesnamelist[i], argv1) == 0){
            free(dbdir->filesnamelist[i]);
            dbdir->index = i;
            
            for(uint32_t i = dbdir->index; i < dbdir->count - 1; i++){
                dbdir->filesnamelist[i] = dbdir->filesnamelist[i + 1];
            }
            dbdir->filesnamelist[dbdir->count - 1] = NULL;

            dbdir->count--;

            char** tmp = realloc(dbdir->filesnamelist,
                dbdir->count * sizeof *dbdir->filesnamelist
            );

            if(tmp != NULL && dbdir->count != 0)
                dbdir->filesnamelist = tmp;


            if(dbdir->currentfilename != NULL && strcmp(dbdir->currentfilename, argv1) == 0){
                free(dbdir->currentfilename);
                dbdir->currentfilename = NULL;

                free(dbdir->currentfilepath);
                dbdir->currentfilepath = NULL;
            }

            return 0;
        }
    }
}

int info_db_file(const DBdir* dbdir){
    if(dbdir == NULL || dbdir->dirpath == NULL){
        printf("Not select dir for database!");
        return 1;
    }

    printf("Directory path: %s\n\n", dbdir->dirpath);

    printf("List of all files:\n\n");
    if(dbdir->count > 0){
        for(uint32_t i = 0; i < dbdir->count; i++){
            char* tmp = malloc(
                strlen(dbdir->dirpath)
                + 1
                + strlen(dbdir->filesnamelist[i])
                + 1
            );

            sprintf(
                tmp,
                "%s/%s",
                dbdir->dirpath,
                dbdir->filesnamelist[i]
            );

            printf("  -   -   FILE  %s: PATH  %s\n",
                dbdir->filesnamelist[i],
                tmp
            );
            free(tmp);
        }
    }
    else
        printf("NULL\n");

    if(dbdir->currentfilename != NULL){
        printf("Current  FILE  %s: PATH  %s\n",
            dbdir->currentfilename,
            dbdir->currentfilepath
        );
    }
    else{
        printf("Current file is NULL!");
    }
}


FILE* db_temp (DBdir* dbdir) 
{
    /*...*/    
}