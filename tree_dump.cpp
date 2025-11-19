#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "tree_dump.h"
#include "tree_funcs.h"

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

    if (tree->err_stat != ok)
    {
        printf_err(debug_mode, "[from verify_tree] -> err_stat is not ok\n");
        error_count++;
    }

    node_count = 0;
    err_t nodes_ok = verify_node(tree->root, debug_mode, tree->size);

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

    if (node->string == NULL)
    {
        printf_err(debug_mode, "[from verify_node] -> node [%p] string not found\n", node);
        return error;
    }

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
            print_tree_dump(tree, "Verification failed\n");
            return error;
            break;
        case ok:
            printf_log_msg(tree->debug_mode, "verification passed\n");
            return ok;
            break;
    };
}


void print_tree_dump(const tree* tree, const char* format, ...)
{
    assert(tree != NULL);

    va_list ap;
    va_start(ap, format);

    md_t debug_mode = tree->debug_mode;

    printf_log_bold(debug_mode, "======================= TREE DUMP =======================\n\n", NULL);

    vprintf(format, ap);
    vfprintf(log_ptr, format, ap);

    printf_log_bold(debug_mode, "\ntree [%p]\n\n", tree);
    printf_log_msg(debug_mode, "\terr_stat   =  %d (0 - no_error, 1 - error)\n", tree->err_stat);
    printf_log_msg(debug_mode, "\tdebug_mode =  %d (0 - off, 1 - on)\n", tree->debug_mode);
    printf_log_msg(debug_mode, "\tsize       =  %zu\n\n", tree->size);

    printf_log_bold(debug_mode, "=========================================================\n\n", NULL);

    generate_dump_image(tree);

    va_end(ap);
}

#define FPRINT(...) fprintf(fp, __VA_ARGS__)

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

    FPRINT("digraph G\n");
    FPRINT("{\n");

    fill_preamble(fp);

    FPRINT("{\n");
    FPRINT("edge[color = \"#45503B\", penwidth = 1]\n");

    if (tree->root != NULL)
         list_nodes(fp, tree->root);

    FPRINT("}\n\n");

    FPRINT("}\n");

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

    // FPRINT("rankdir = LR;\n");
    FPRINT("bgcolor = \"white\"\n");
    FPRINT("node[shape = doubleoctagon, style = \"filled\", fillcolor = \"red\", fontcolor = \"white\"]\n");
}


#define PRINT_PTR(PTR) FPRINT("<FONT face = \"monospace\" color=\"#%x\"> %p </FONT>", hash((long long int) PTR), PTR)
#define BEGIN_ROW FPRINT("<TR>")
#define END_ROW FPRINT("</TR>")

const node* list_nodes(FILE* fp, const node* current_node)
{
    assert(fp != NULL);
    assert(current_node != NULL);

    //printf("listing node %p\n", current_node);

    FPRINT("NODE_%p [label = <", current_node);
    FPRINT("<TABLE BORDER=\"0\" CELLBORDER=\"1\">");

    BEGIN_ROW;
    FPRINT("<TD COLSPAN=\"2\">"); FPRINT("ptr ="); PRINT_PTR(current_node); FPRINT("</TD>");
    END_ROW;

    BEGIN_ROW;
    FPRINT("<TD COLSPAN=\"2\">"); FPRINT("%s", current_node->string); FPRINT("</TD>");
    END_ROW;

    BEGIN_ROW;
    FPRINT("<TD PORT=\"f0\">"); FPRINT("yes ["); PRINT_PTR(current_node->yes_branch); FPRINT("] </TD>");
    FPRINT("<TD PORT=\"f1\">"); FPRINT("no [");  PRINT_PTR(current_node->no_branch);  FPRINT("] </TD>");
    END_ROW;

    FPRINT("</TABLE>>, shape = plain, style = filled, fillcolor = \"#C0C0C0\", fontcolor = \"black\"]\n");
        
    if (current_node->yes_branch != NULL)
    {
        const node* next_node = list_nodes(fp, current_node->yes_branch);
        FPRINT("NODE_%p:f0 -> NODE_%p\n", current_node, next_node);
    }

    if (current_node->no_branch != NULL)
    {
        const node* next_node = list_nodes(fp, current_node->no_branch);
        FPRINT("NODE_%p:f1 -> NODE_%p\n", current_node, next_node);
    }

    return current_node;    
}

#undef FPRINT
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

