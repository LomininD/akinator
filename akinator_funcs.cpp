#include <stdio.h>
#include "akinator_funcs.h"


void request_cmd(tree* tree)
{
    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "request_cmd: began requesting command\n");
    printf_both(debug_mode, "-> Choose a command to execute");
    printf_both(debug_mode, "-> [g]uess object, [s]ave base, [l]oad base, [q]uit");

    
}

err_t request_tree_beginning(tree* tree)
{
    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "request_new_node: began requesting tree beginning\n");

    printf_both(debug_mode, "-> Enter first object name, please:\n");
    char* first_obj_name = request_string(debug_mode);
    if (first_obj_name == NULL)
        return error;
    
    printf_both(debug_mode, "-> Enter second object name, please:\n");
    char* second_obj_name = request_string(debug_mode);
    if (second_obj_name == NULL)
        return error;

    node* yes_node  = create_node(debug_mode);
    node* no_node = create_node(debug_mode);

    yes_node->string  = first_obj_name;
    no_node->string = second_obj_name;

    printf_both(debug_mode, "-> How does %s differ from %s?\n", first_obj_name, second_obj_name);
    char* diff_str = request_string(debug_mode);
    
    node* root_node = create_node(debug_mode);
    root_node->string = diff_str;
    root_node->yes_branch = yes_node;
    root_node->no_branch = no_node;

    tree->root = root_node;
    tree->size = 3;
    
    VERIFY_TREE(error);

    printf_log_msg(debug_mode, "request_new_node: tree beginning requested\n");

    DISPLAY_TREE();

    return ok;
}


err_t request_new_nodes(tree* tree, node* parent_node)
{
    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "request_new_node: began requesting new nodes\n");

    printf_both(debug_mode, "-> What is it?\n");

    char* obj_name  = request_string(debug_mode);

    if (obj_name == NULL)
        return error;

    printf_both(debug_mode, "-> How does it differ form %s?\n", parent_node->string);

    char* diff_string  = request_string(debug_mode);

    if (diff_string == NULL)
        return error;

    node* new_left_node = create_node(debug_mode);
    node* new_right_node = create_node(debug_mode);

    new_right_node->string = parent_node->string;
    new_left_node->string = obj_name;
    parent_node->string = diff_string;
    parent_node->yes_branch = new_left_node;
    parent_node->no_branch = new_right_node;

    tree->size += 2;

    VERIFY_TREE(error);
    
    printf_log_msg(debug_mode, "request_new_node: new nodes are requested\n");

    DISPLAY_TREE()
    return ok;
}
