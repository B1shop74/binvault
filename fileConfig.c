#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "fileConfig.h"


typedef struct{
    char* fname;
    char* filepath;
    char mode;
}DBfile;


DBfile* set_db_file(const char* argv1){
    if(argv1 == NULL)
        return NULL;

    DBfile* dbfile;

    dbfile = malloc(sizeof(DBfile));

    dbfile->filepath = malloc(sizeof(argv1));
    strcpy(dbfile->filepath, argv1);

    FILE* db = fopen(dbfile->filepath, "rb");
    if(db == NULL){
        printf("Uncorect file path!");
        free(dbfile->filepath);
        free(dbfile);
        return NULL;
    }
    else{
        dbfile->mode = 'r';

        dbfile->fname = strrchr(dbfile->filepath, '/');

        if(dbfile->fname != NULL)
            dbfile->fname++;
        else{
            printf("False path!");
            free(dbfile->filepath);
            free(dbfile);
            return NULL;
        }

        fclose(db);
        return dbfile;
    }
}

int rename_db_file(DBfile* dbfile, const char* argv1){
    if(argv1 == NULL)
        return NULL;

    FILE* db = fopen(argv1, "rb");
    if(db == NULL)
        return NULL;

    rename(dbfile->filepath, argv1);

    dbfile->fname = strrchr(dbfile->filepath, '/');

        if(dbfile->fname != NULL){
            dbfile->fname++;
            printf("File renamed by: %s", dbfile->fname);
            return 0;
        }    
        else{
            printf("False path!");
            return NULL;
        }
}

int remove_db_file(DBfile* dbfile){
    if(dbfile == NULL)
        return NULL;

    remove(dbfile->filepath);
    free(dbfile->filepath);
    free(dbfile);

    dbfile = NULL;
}

