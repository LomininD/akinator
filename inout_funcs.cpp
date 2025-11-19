#include <stdlib.h>
#include <string.h>
#include "inout_funcs.h"

static void clear_buffer(void);
static bool check_buffer();


ans_t get_answer(md_t debug_mode)
{
    char ans = 0;
    bool got_ans = false;
    int scanned = 0;

    while(!got_ans)
    {
        scanned = scanf("%c", &ans);

        if (scanned != 1 || !check_buffer())
        {
            printf_both(debug_mode, "-> Could not read the answer, please try again:\n");
            clear_buffer();
            clearerr(stdin);
            continue;
        }

        printf_log_msg(debug_mode, "get_answer: got \"%c\"\n", ans);

        if (ans == 'y')
        {
            //clear_buffer();
            return yes;
        }
        else if (ans == 'n')
        {
            //clear_buffer();
            return no;
        }
        else
        {
            printf_both(debug_mode, "-> Could not recognize the answer, please try again:\n");
            //clear_buffer();
            continue;
        }
    }

    return yes;
}


// FIXME - should ask again if got only '\n' remove excess spaces
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
        printf_both(debug_mode, "-> Could not recognize the answer, please try again\n");
        clearerr(stdin);
        read = getline(&string, &string_len, stdin); // clear_buffer()?
    }

    *strrchr(string, '\n') = '\0';

    printf_log_msg(debug_mode, "request_string: got \"%s\"\n", string);

    return string;
}

static void clear_buffer(void)
{
    int c = '\0';
    while ((c = getchar()) != '\n' && c != EOF);
    //printf("clear_buffer: cleared\n");
}

static bool check_buffer()
{
    int c = 0;
    while ((c = getchar()) != EOF && c != '\n')
        if (c != ' ' && c != '\n' && c != '\t')
            return false;
    return true;
}
