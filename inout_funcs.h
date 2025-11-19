#ifndef  INOUT_FUNCS_H
#define INOUT_FUNCS_H

#include "debug.h"

enum cmd_t
{
    guess,
    save,
    load,
    quit,

    unknown
};

enum ans_t
{
    no,
    yes,

    no_ans
};

cmd_t get_cmd(md_t debug_mode);
ans_t get_answer(md_t debug_mode);
char* request_string(md_t debug_mode);

#endif