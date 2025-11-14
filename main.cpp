#include <stdio.h>
#include "tree_properties.h"
#include "tree_funcs.h"
#include "tree_dump.h"
#include <stdlib.h>
#include <string.h>

int main()
{
    md_t debug_mode = on;
    initialize_log(debug_mode, "akinator_log.html", "AKINATOR_LOG");
    
    // sample tree structure

    char* str_1 = (char*) calloc(100, sizeof(char*));
    char* str_2 = (char*) calloc(100, sizeof(char*));
    char* str_3 = (char*) calloc(100, sizeof(char*));
    char* str_4 = (char*) calloc(100, sizeof(char*));
    char* str_5 = (char*) calloc(100, sizeof(char*));
    char* str_6 = (char*) calloc(100, sizeof(char*));
    char* str_7 = (char*) calloc(100, sizeof(char*));

    str_1 = strdup("Kirikov");
    str_2 = strdup("Divari");
    str_3 = strdup("Petrovic");
    str_4 = strdup("gives homework");
    str_5 = strdup("teaches matan");
    str_6 = strdup("Poltorashka");
    str_7 = strdup("Animal");

    
    node node_6 = {str_1, NULL, NULL};
    node node_16 = {str_2, NULL, NULL};
    node node_15 = {str_3, NULL, NULL};
    node node_3 = {str_4, &node_6, &node_16};
    node node_20 = {str_5, &node_15, &node_3};
    node node_5 = {str_6, NULL, NULL};
    node root_node = {str_7, &node_5, &node_20};

    tree ref_tree = {&root_node, 7, debug_mode, ok};

    print_tree_dump(&ref_tree);

    bool end = false;
    ans_t ans = yes;

    while (!end)
    {
    bool guessed = false;
    node* current_node = ref_tree.root;

        while(!guessed)
        {
            printf_both(debug_mode, "%s? ([y]es / [n]o)\n", current_node->string);

            ans = get_answer(debug_mode);

            if (ans == yes)
                current_node = current_node->yes_branch;
            else 
                current_node = current_node->no_branch;
            

            if (current_node->yes_branch == NULL)   
            {
                printf_both(debug_mode, "This is %s, am I right? ([y]es / [n]o)\n", current_node->string);
                ans = get_answer(debug_mode);
                if (ans == yes)
                {
                    printf_both(debug_mode, "Yeah, I am always right.\n");
                }
                else
                {
                    //printf_both(debug_mode, "What is it?\n");
                    err_t requested = request_new_nodes(&ref_tree, current_node);
                    if (requested != ok)
                        break;
                }
                guessed = true;
            }
        }

        print_tree_dump(&ref_tree);

        printf_both(debug_mode, MAKE_BOLD("Would you like to try again? ([y]es / [n]o)\n"));

        ans = get_answer(debug_mode);

        if (ans == no)
            end = true;
    }


    print_tree_dump(&ref_tree);
    destroy_tree(&ref_tree);
    fclose(log_ptr);
    return 0;
}
