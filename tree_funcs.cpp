#include "tree_funcs.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


static node* create_node(md_t debug_mode);
static err_t delete_node(tree* tree, node* node);

static char* request_string(md_t debug_mode);
static void clear_buffer(void);


void initialize_tree_log(md_t debug_mode)
{
    log_ptr = fopen("akinator_log.html", "w");
    printf_log_only(debug_mode, "<pre>\n");
    printf_log_only(debug_mode, "<h3> +++ AKINATOR LOG +++ </h3>\n");
}


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


ans_t get_answer(md_t debug_mode)
{
    char ans = 0;
    bool got_ans = false;
    int scanned = 0;

    while(!got_ans)
    {
        scanned = scanf("%c", &ans);

        if (scanned != 1)
        {
            printf_both(debug_mode, "Could not read the answer, please try again:\n");
            clearerr(stdin);
            continue;
        }

        printf_log_msg(debug_mode, "get_answer: got %c\n", ans);

        if (ans == 'y')
        {
            clear_buffer();
            return yes;
        }
        else if (ans == 'n')
        {
            clear_buffer();
            return no;
        }
        else
        {
            printf_both(debug_mode, "Could not recognize the answer, please try again:\n");
            clear_buffer();
            continue;
        }
    }

    return yes;
}

// FIXME - should ask again if got only '\n' 
char* request_string(md_t debug_mode) 
{
    char* string = (char*) calloc(100, sizeof(string));

    if (string == NULL)
    {
        printf_err(debug_mode, "[from request_string] -> could not allocate memory for node data\n");
        return NULL;
    }

    size_t string_len = 0;
    int read = 0;
    read = getline(&string, &string_len, stdin);
    
    while (read <= 0)
    {
        printf_both(debug_mode, "Could not recognize the answer, please try again\n");
        clearerr(stdin);
        read = getline(&string, &string_len, stdin); // clear_buffer()?
    }

    *strrchr(string, '\n') = '\0';

    printf_log_msg(debug_mode, "request_string: got %s\n", string);

    return string;
}


err_t request_new_nodes(tree* tree, node* parent_node)
{
    VERIFY_TREE(error);

    md_t debug_mode = tree->debug_mode;

    printf_log_msg(debug_mode, "request_new_node: began requesting new nodes\n");

    printf_both(debug_mode, "What is it?\n");

    char* obj_name  = request_string(debug_mode);

    if (obj_name == NULL)
        return error;

    printf_both(debug_mode, "How does it differ form %s?\n", parent_node->string);

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
    return ok;
}


node* create_node(md_t debug_mode)
{
    printf_log_msg(debug_mode, "create_node: began creating node\n");

    node* new_node = (node*) calloc(1, sizeof(node));

    printf_log_msg(debug_mode, "create_node: done creating node [%p]\n", new_node);
    return new_node;
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

static void clear_buffer(void)
{
    int c = '\0';
    while ((c = getchar()) != '\n' && c != EOF);
    printf("clear_buffer: cleared\n");
}