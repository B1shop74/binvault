#ifndef FILECONFIG_H
#define FILECONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char *dirpath;
    char *currentfilepath;
    char *currentfilename;
    char **filesnamelist;

    int32_t index;
    uint32_t count;

    bool is_listVlb;
    bool is_dirSct;
} DBdir;


DBdir *set_db_dir(
    const char *argv1
);

int set_db_file(
    DBdir *dbdir,
    const char **argv1
);

int set_current_file(
    DBdir *dbdir,
    const char *argv1
);

int rename_db_file(
    DBdir *dbdir,
    const char *argv1old,
    const char *argv1new
);

int remove_db_file(
    DBdir* dbdir,
    const char* argv1
);

int change_db_dir(
    DBdir* dbdir, 
    const char* argv1
);

int info_db_file(
    const DBdir* dbdir
);

#endif