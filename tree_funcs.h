#ifndef TREE_FUNCS_H
#define TREE_FUNCS_H

#include "tree_properties.h"
#include "tree_dump.h" 

enum tree_view
{
    preorder,
    postorder,
    inorder
};

node* create_node(md_t debug_mode);
err_t tree_ctor(tree* tree, md_t debug_mode);
void destroy_tree(tree* tree);

err_t print_tree(const tree* tree, const node* root, tree_view print_type);
err_t print_node(const node* node, tree_view print_type, md_t debug_mode);


#ifdef TREE_VERIFICATION
#define VERIFY_TREE(RET) do{                    \
    if (process_tree_verification(tree) != ok)  \
        return RET;                             \
}                                               \
while(0)

#else

# define VERIFY_TREE(RET) do{                   \
    if (tree == NULL)                           \
        return RET;                             \
}                                               \
while(0);

#endif

#define DISPLAY_TREE() {                \
    if (debug_mode == on)               \
        generate_dump_image(tree);      \
}

#define print_data_if(COND, ...) do{                        \
    if (print_type == COND)                                 \
        printf_both(debug_mode, __VA_ARGS__);               \
}                                                           \
while(0)

#endif