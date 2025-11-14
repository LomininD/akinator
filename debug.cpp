#include "debug.h"

FILE* log_ptr = NULL;


void initialize_log(md_t debug_mode, const char* file_name, const char* log_title)
{
    log_ptr = fopen(file_name, "w");
    printf_log_only(debug_mode, "<pre>\n");
    printf_log_only(debug_mode, "<h3> +++ %s +++ </h3>\n", log_title);
}