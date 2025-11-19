#include "akinator_funcs.h"


static err_t check_prediction(tree* tree, node* current_node);
static void write_node(FILE* save_ptr, const tree* tree, const node* current_node);


cmd_t request_cmd(const tree* tree)
{
    VERIFY_TREE(unknown);
    
    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "\n");
    printf_log_msg(debug_mode, "request_cmd: began requesting command\n");

    printf_both(debug_mode, "-> Choose a command to execute\n");
    printf_both(debug_mode, "-> [g]uess object, [s]ave base, [l]oad base, [q]uit\n");

    cmd_t current_cmd = get_cmd(debug_mode);

    printf_log_msg(debug_mode, "request_cmd: finished requesting command\n\n");

    return current_cmd;
}


err_t process_guessing(tree* tree)
{
    VERIFY_TREE(error); // FIXME - copy pasting

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "guess_object: began guessing object\n");
    
    if (tree->size == 0)
    {
        err_t requested_beginning = request_tree_beginning(tree);
        if (requested_beginning != ok) return requested_beginning;
    }

    ans_t ans = no_ans;
    bool guessed = false;
    node* current_node = tree->root;

    while(!guessed)
    {
        printf_both(debug_mode, "-> %s? ([y]es / [n]o)\n", current_node->string);

        ans = get_answer(debug_mode);

        if (ans == yes) current_node = current_node->yes_branch;
        else            current_node = current_node->no_branch;

        if (current_node->yes_branch == NULL)   
        {
            err_t checked = check_prediction(tree, current_node);
            if (checked != ok) return checked;
            guessed = true;
        }
    }

    VERIFY_TREE(error);

    printf_log_msg(debug_mode, "guess_object: finished guessing object\n");

    return ok;
}


err_t save_database(const tree* tree)
{
    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "save_data_base: began saving database\n");

    printf_both(debug_mode, "-> Saving current version of database may overwrite previous saved version of database\n");
    printf_both(debug_mode, "-> Save anyway? ([y]es / [n]o)\n");

    ans_t ans = get_answer(debug_mode);
    FILE* save_ptr = NULL;

    switch (ans)
    {
        case yes:
            printf_log_msg(debug_mode, "\n");
            printf_log_msg(debug_mode, "save_database: started writing in file\n");
            save_ptr = fopen(SAVE_FILE_NAME, "w");
            write_node(save_ptr, tree, tree->root);
            fclose(save_ptr);
            printf_log_msg(debug_mode, "save_database: done writing in file\n\n");
            printf_both(debug_mode, "-> Database saved successfully\n");
            break;
        case no:
            break;
        default:
            return error;
    };

    printf_log_msg(debug_mode, "save_data_base: database saved\n");

    return ok;
}

#define FPRINT(...) fprintf(save_ptr, __VA_ARGS__)

void write_node(FILE* save_ptr, const tree* tree, const node* current_node)
{
    printf_log_msg(tree->debug_mode, "writing [%p]\n", current_node);

    if (current_node == NULL) { FPRINT("nil "); return; }
    
    FPRINT("( ");
    FPRINT("\"%s\" ", current_node->string);
    write_node(save_ptr, tree, current_node->yes_branch);
    write_node(save_ptr, tree, current_node->no_branch);
    FPRINT(") ");
}

#undef FPRINT


err_t check_prediction(tree* tree, node* current_node)
{
    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "check_prediction: began checking guessed object\n");

    printf_both(debug_mode, "-> This is %s, am I right? ([y]es / [n]o)\n", current_node->string);
    ans_t ans = get_answer(debug_mode);
    err_t requested = ok; // FIXME - not supposed to be here

    switch (ans)
    {
        case yes:
            printf_both(debug_mode, "-> Yeah, I am always right.\n");
            return ok;
        case no:
            requested = request_new_nodes(tree, current_node);
            if (requested != ok) return requested;
            return ok;
        default:
            return error;
    };
}


err_t request_tree_beginning(tree* tree)
{
    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "request_new_node: began requesting tree beginning\n");

    printf_both(debug_mode, "-> Hm... It seems that there is no data, where can it be?\n");
    printf_both(debug_mode, "-> Lets create and set up data base now.\n");

    printf_both(debug_mode, "-> Enter first object name, please:\n"); // FIXME - copy pasting
    char* first_obj_name = request_string(debug_mode);
    if (first_obj_name == NULL) return error;
    
    printf_both(debug_mode, "-> Enter second object name, please:\n");
    char* second_obj_name = request_string(debug_mode);
    if (second_obj_name == NULL) return error;

    node* yes_node = create_node(debug_mode);
    node* no_node  = create_node(debug_mode);

    yes_node->string = first_obj_name;
    no_node->string  = second_obj_name;

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
