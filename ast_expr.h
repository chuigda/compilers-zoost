#ifndef AST_EXPR_H
#define AST_EXPR_H

#include "lex.h"
#include "c_list.h"
#include "env.h"

#ifdef __cplusplus
extern "C" {
#endif

enum c_expr_factor_type
{
    ceft_id,
    ceft_number,
};

typedef struct
{
    union
    {
        int id;
        int number;
    }
    variant;
    int factor_type;
} c_expr_factor;

enum c_expr_operator_type
{
    ceot_add     = '+',
    ceot_sub     = '-',
    ceot_mul     = '*',
    ceot_div     = '/',
    ceot_and     = '&',
    ceot_or      = '|',
    ceot_lt      = '<',
    ceot_gt      = '>',
    ceot_eq      = '=',
    ceot_factor  = 0
};

typedef struct c_expr_operator_struct
{
    union
    {
        struct
        {
            struct c_expr_operator_struct *left_child;
            struct c_expr_operator_struct *right_child;
        }
        binary_op_childs;
        c_expr_factor *single_factor;
    }
    variant;
    int operator_type;
} c_expr_operator, c_expr;

extern c_expr* g_parse_expr(c_list *_tokens, int *_index, c_env *_env);
extern void g_delete_expr(c_expr* _expr);

#ifdef __cplusplus
}
#endif
#endif // AST_EXPR_H
