#ifndef  INOUT_FUNCS_H
#define INOUT_FUNCS_H

#include "debug.h"

enum ans_t
{
    no,
    yes
};

ans_t get_answer(md_t debug_mode);
char* request_string(md_t debug_mode);

#endif