#include "tree_funcs.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


static void destroy_node(node* node, md_t debug_mode);


err_t tree_ctor(tree* tree, md_t debug_mode)
{
    if (tree == NULL)
    {
        printf_err(debug_mode, "[from tree_ctor] -> tree not found\n");
        return error;
    }

    printf_log_msg(debug_mode, "tree_ctor: began initialising tree [%p]", tree);

    tree->debug_mode = debug_mode;

    VERIFY_TREE(error);

    printf_log_msg(debug_mode, "tree_ctor: finished initialising tree [%p]", tree);
    return ok;
}


node* create_node(md_t debug_mode)
{
    printf_log_msg(debug_mode, "create_node: began creating node\n");

    node* new_node = (node*) calloc(1, sizeof(node));

    new_node->freshly_created = true;

    printf_log_msg(debug_mode, "create_node: done creating node [%p]\n", new_node);
    return new_node;
}


void destroy_tree(tree* tree)
{
    assert(tree != NULL);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "destroy_tree: began cutting down tree\n");

    destroy_node(tree->root, debug_mode);

    tree->root = NULL;

    printf_log_msg(debug_mode, "destroy_tree: finished cutting down tree\n");
}


// change when read from file added !!!
void destroy_node(node* node, md_t debug_mode)
{
    assert(node != NULL);
    printf_log_msg(debug_mode, "deleting %p\n", node);

    if (node->yes_branch != NULL)
    {
        destroy_node(node->yes_branch, debug_mode);
    }
    if (node->no_branch)
    {
        destroy_node(node->no_branch, debug_mode);
    }

    free(node->string);

    if (node->freshly_created)
    {
        free(node);
    }
}


err_t print_tree(const tree* tree, const node* root, tree_view print_type)
{
    assert(root != NULL);
    assert(tree != NULL);

    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "print_tree: began printing tree from node [%p]\n\n", root);

    err_t printed = print_node(root, print_type, debug_mode);
    if (printed != ok)
    {
        return printed;
    }

    printf_both(debug_mode, "\n");
    printf_log_msg(debug_mode, "\nprint_tree: done printing tree\n");
    return ok;
}

err_t print_node(const node* node, tree_view print_type, md_t debug_mode)
{
    assert(node != NULL);

    printf_log_msg(debug_mode, "(");

    print_data_if(preorder, "%s ", node->string);

    if (node->yes_branch != NULL)
        print_node(node->yes_branch, print_type, debug_mode);

    print_data_if(inorder, "%s ", node->string);
    
    if (node->no_branch != NULL)
        print_node(node->no_branch, print_type, debug_mode);

    print_data_if(postorder, "%s ", node->string);

    printf_log_msg(debug_mode, ")");
    return ok;
}
