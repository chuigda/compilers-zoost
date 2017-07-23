#ifndef LEX_H
#define LEX_H

#include "c_string.h"
#include "c_list.h"

#ifdef __cplusplus
extern "C" {
#endif

enum c_token_type
{
    token_rwd_input = 0,
    token_rwd_print = 1,
    token_rwd_goto  = 2,
    token_rwd_flag  = 3,
    token_rwd_if    = 4,
    token_rwd_let   = 5,
    token_rwd_var   = 6,

    token_sym_add        = '+',
    token_sym_sub        = '-',
    token_sym_mul        = '*',
    token_sym_div        = '/',
    token_sym_lbracket   = '[',
    token_sym_rbracket   = ']',
    token_sym_comma      = ',',
    token_sym_semicolon  = ';',
    token_sym_lt         = '<',
    token_sym_gt         = '>',
    token_sym_eq         = '=',
    token_sym_and        = '&',
    token_sym_or         = '|',
    token_sym_not        = '!',

    token_number = 7,
    token_string = 8
};

typedef struct
{
    int token_type;

    union
    {
        c_string *string;
        int number;
    } value;

    int line;
}
c_token;

extern c_list* g_scan_and_split(const c_string* _src_code);
extern void g_delete_token(c_token* _token);

extern void g_dump_token(const c_token* _token);

#ifdef __cplusplus
}
#endif

#endif // LEX_H
