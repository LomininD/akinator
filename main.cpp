#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree_properties.h"
#include "tree_funcs.h"
#include "tree_dump.h"
#include "akinator_funcs.h"

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

    
    node node_6 = {str_1, NULL, NULL, false};
    node node_16 = {str_2, NULL, NULL, false};
    node node_15 = {str_3, NULL, NULL, false};
    node node_3 = {str_4, &node_6, &node_16, false};
    node node_20 = {str_5, &node_15, &node_3, false};
    node node_5 = {str_6, NULL, NULL, false};
    node root_node = {str_7, &node_5, &node_20, false};

    tree ref_tree = {&root_node, 7, debug_mode, ok};

    print_tree_dump(&ref_tree, "Beginning tree view\n");

    cmd_t current_cmd = unknown;
    bool end = false;

    do
    {
        current_cmd = request_cmd(&ref_tree);
        switch (current_cmd)
        {
            case guess:
                process_guessing(&ref_tree);
                break;
            case load:
                break;
            case save:
                break;
            case quit:
                end = true;
                break;
            default:
                printf_err(debug_mode, "[from main] -> could not recognize the command\n");
                break;
        };
    }
    while (!end);

    print_tree_dump(&ref_tree, "Finish tree view\n");
    destroy_tree(&ref_tree);
    fclose(log_ptr);
    return 0;
}
