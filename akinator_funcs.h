#ifndef  AKINATOR_FUNCS_H
#define AKINATOR_FUNCS_H

#include "debug.h"
#include "tree_funcs.h"
#include "inout_funcs.h"


err_t request_tree_beginning(tree* tree);
err_t request_new_nodes(tree* tree, node* parent_node);


#endif