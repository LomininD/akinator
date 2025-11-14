#include "tree_dump.h"
#include "tree_funcs.h"
#include <stdio.h>
#include <assert.h>

size_t node_count = 0;

static int hash(long long int ptr);
static void convert_to_image(char* code_file_name, char* image_file_name);
static void fill_preamble(FILE* fp);
static const node* list_nodes(FILE* fp, const node* current_node);
static err_t verify_node(const node* node, md_t debug_mode, size_t max_size);

err_t verify_tree(const tree* tree)
{
    if (tree == NULL)
    {
        printf(MAKE_BOLD_RED("ERROR:") "[from verify_tree] -> tree not found\n");
        return no_data;
    }

    md_t debug_mode = tree->debug_mode;

    int error_count = 0;
    node_count = 0;
    err_t nodes_ok = verify_node(tree->root, debug_mode, tree->size);

    if (tree->err_stat != ok)
    {
        printf_err(debug_mode, "[from verify_tree] -> err_stat is not ok\n");
        error_count++;
    }

    if (nodes_ok != ok)
    {
        error_count++;
    }

    if (node_count != tree->size)
    {
        printf_err(debug_mode, "[from verify_tree] -> wrong size (should be %zu, but found %zu elements)",\
                                                                         tree->size, node_count);
        error_count++;
    }

    if (error_count != 0)
    {
        struct tree* unlocked_tree = (struct tree*) tree; // to make possible to change err_stat
        unlocked_tree->err_stat = error;
        return error;
    }

    return ok;
}


err_t verify_node(const node* node, md_t debug_mode, size_t max_size)
{
    if (node == NULL)
        return ok;

    node_count++;

    if (node_count > max_size)
    {
        printf_err(debug_mode, "[from verify_node] -> number of nodes is greater than actual size %zu\n", max_size);
        return error;
    }

    // if ()
    // {
    //     printf_err(debug_mode, "[from verify_node] -> node [%p] string not found\n", node);
    //     return error;
    // }

    if (verify_node(node->yes_branch,  debug_mode, max_size) == ok && \
        verify_node(node->no_branch, debug_mode, max_size) == ok)
    {
        return ok;
    }
    else
    {
        return error;
    }
}


err_t process_tree_verification(const tree* tree)
{
    err_t verified = verify_tree(tree);

    switch (verified)
    {
        case no_data:
            printf(MAKE_BOLD_RED("verification failed") "not enough data to show additional information\n");
            return error;
            break;
        case error:
            printf_err(tree->debug_mode, "verification failed\n"); 
            print_tree_dump(tree);
            return error;
            break;
        case ok:
            printf_log_msg(tree->debug_mode, "verification passed\n");
            return ok;
            break;
    };
}


void print_tree_dump(const tree* tree)
{
    assert(tree != NULL);

    md_t debug_mode = tree->debug_mode;

    printf_log_bold(debug_mode, "======================= TREE DUMP =======================\n\n", NULL);

    printf_log_bold(debug_mode, "tree [%p]\n\n", tree);
    printf_log_msg(debug_mode, "\terr_stat   =  %d (0 - no_error, 1 - error)\n", tree->err_stat);
    printf_log_msg(debug_mode, "\tdebug_mode =  %d (0 - off, 1 - on)\n", tree->debug_mode);
    printf_log_msg(debug_mode, "\tsize       =  %zu\n\n", tree->size);

    printf_log_bold(debug_mode, "=========================================================\n\n", NULL);

    generate_dump_image(tree);
}


// generates tree graph and puts in html log file
void generate_dump_image(const tree* tree)
{
    assert(tree != NULL);

    if (tree->debug_mode == off)
        return;

    static unsigned long long image_count = 0; // size_t?

    char code_file_name[file_name_size] = {};
    char image_file_name[file_name_size] = {};
    snprintf(code_file_name, file_name_size, IMAGE_CODE_PATH "%llu." IMAGE_CODE_EXT, image_count);
    snprintf(image_file_name, file_name_size, IMAGE_PATH "%llu." IMAGE_EXT, image_count);

    FILE* fp = fopen(code_file_name, "w");

    fprintf(fp, "digraph G\n");
    fprintf(fp, "{\n");

    fill_preamble(fp);

    fprintf(fp, "{\n");
    fprintf(fp, "edge[color = \"#45503B\", penwidth = 1]\n");

    if (tree->root != NULL)
         list_nodes(fp, tree->root);

    fprintf(fp, "}\n\n");

    fprintf(fp, "}\n");

    fclose(fp);
    image_count++;

    convert_to_image(code_file_name, image_file_name);

    printf_log_only(tree->debug_mode, "\n\n <img src=\"%s\"> \n\n", image_file_name);
}


void convert_to_image(char* code_file_name, char* image_file_name)
{
    assert(code_file_name != NULL);
    assert(image_file_name != NULL);

    char sys_str[sys_str_size] = {};
    snprintf(sys_str, sys_str_size, "dot %s -T " IMAGE_EXT " -o %s", code_file_name, image_file_name); 

    //puts(sys_str);

    system(sys_str);
}


void fill_preamble(FILE* fp)
{
    assert(fp != NULL);

    // fprintf(fp, "rankdir = LR;\n");
    fprintf(fp, "bgcolor = \"white\"\n");
    fprintf(fp, "node[shape = doubleoctagon, style = \"filled\", fillcolor = \"red\", fontcolor = \"white\"]\n");
}


#define PRINT_PTR(PTR) fprintf(fp, "<FONT face = \"mono\" color=\"#%x\"> %p </FONT>", hash((long long int) PTR), PTR)
#define BEGIN_ROW fprintf(fp, "<TR>")
#define END_ROW fprintf(fp, "</TR>")

const node* list_nodes(FILE* fp, const node* current_node)
{
    assert(fp != NULL);
    assert(current_node != NULL);

    //printf("listing node %p\n", current_node);

    fprintf(fp, "NODE_%p [label = <", current_node);
    fprintf(fp, "<TABLE BORDER=\"0\" CELLBORDER=\"1\">");

    BEGIN_ROW;
    fprintf(fp, "<TD COLSPAN=\"2\">"); fprintf(fp, "ptr ="); PRINT_PTR(current_node); fprintf(fp, "</TD>");
    END_ROW;

    BEGIN_ROW;
    fprintf(fp, "<TD COLSPAN=\"2\">"); fprintf(fp, "%s", current_node->string); fprintf(fp, "</TD>");
    END_ROW;

    BEGIN_ROW;
    fprintf(fp, "<TD PORT=\"f0\">"); fprintf(fp, "yes ["); PRINT_PTR(current_node->yes_branch); fprintf(fp, "] </TD>");
    fprintf(fp, "<TD PORT=\"f1\">"); fprintf(fp, "no [");  PRINT_PTR(current_node->no_branch);  fprintf(fp, "] </TD>");
    END_ROW;

    fprintf(fp, "</TABLE>>, shape = plain, style = filled, " \
                "fillcolor = \"#C0C0C0\", fontcolor = \"black\"]\n");
        
    if (current_node->yes_branch != NULL)
    {
        const node* next_node = list_nodes(fp, current_node->yes_branch);
        fprintf(fp, "NODE_%p:f0 -> NODE_%p\n", current_node, next_node);
    }

    if (current_node->no_branch != NULL)
    {
        const node* next_node = list_nodes(fp, current_node->no_branch);
        fprintf(fp, "NODE_%p:f1 -> NODE_%p\n", current_node, next_node);
    }

    return current_node;    
}

#undef PRINT_PTR
#undef BEGIN_ROW
#undef END_ROW

int hash(long long int ptr)
{
    int hashed = 0;

    if (ptr == 0)
    {
        return 16711680;
    }

    while (ptr != 0)
    {
        hashed += (7717 * (ptr % 10) % 1013 + 349) * 147867;
        ptr /= 10; 
    }

    hashed %= 16777215;

    return hashed;
}

