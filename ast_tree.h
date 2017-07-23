#ifndef C_AST_TREE_H
#define C_AST_TREE_H

#include "c_list.h"
#include "c_string.h"
#include "env.h"
#include "ast_expr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct c_stmt_struct;

typedef struct
{
    c_list *id_list;
}
c_var_stmt,
c_input_stmt,
c_print_stmt;

typedef struct
{
    c_expr *condition;
    struct c_stmt_struct *execution;
}
c_if_stmt;

typedef struct
{
    int id;
    c_expr *expr;
}
c_let_stmt;

typedef struct
{
    int id;
}
c_goto_stmt, c_flag_stmt;

enum c_statement_type
{
    cst_invalid,

    cst_var_stmt,
    cst_input_stmt,
    cst_print_stmt,
    cst_let_stmt,
    cst_if_stmt,
    cst_flag_stmt,
    cst_goto_stmt,
    cst_stmt_block
};

typedef struct
{
    c_env *env;
    c_list *stmt_list;
} c_block;

typedef struct c_stmt_struct
{
    union
    {
        c_var_stmt *var_stmt; c_let_stmt *let_stmt;
        c_if_stmt *if_stmt;
        c_input_stmt *input_stmt;
        c_print_stmt *print_stmt;
        c_flag_stmt *flag_stmt;
        c_goto_stmt *goto_stmt;
        c_block *block;
        c_string *label;
    }
    variant;

    int stmt_type;
} c_stmt;

typedef struct
{
    c_env *global_env;
    c_list *statements;
}c_ast_translation_unit;

extern c_ast_translation_unit *g_parse_tokens(c_list *_tokens);
extern void g_delete_ast(c_ast_translation_unit* _ast);

#ifdef __cplusplus
}
#endif

#endif // C_AST_TREE_H
