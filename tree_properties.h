#ifndef TREE_PROPERTIES_H
#define TREE_PROPERTIES_H

#include "debug.h"

#define SAVE_FILE_NAME "database_save.txt"
#define TREE_VERIFICATION

struct node
{
    char* string;
    node* yes_branch;
    node* no_branch;
    bool freshly_created;
};

struct tree
{
    node* root;
    size_t size;
    md_t debug_mode;
    err_t err_stat;
    bool saved;
};

#endif