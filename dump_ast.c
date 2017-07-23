#include "dump_ast.h"

#include <stdio.h>

#define BLOCK_INDENT_SIZE 4
#define STMT_INDENT_SIZE 2

static void dump_expr(c_expr *expr, int _scope_count);
static void dump_statement_list(c_list *_stmt_list, int _scope_count);
static void dump_statement(c_stmt *_stmt, int _scope_count);
static void dump_env(c_env *_env, int _scope_count);
static void print_space(int _count);

extern void dump_ast(c_ast_translation_unit *_trans_unit)
{
    dump_env(_trans_unit->global_env, 0);
    dump_statement_list(_trans_unit->statements, 0);
}

static void dump_expr(c_expr *expr, int _scope_count)
{
    print_space(_scope_count * BLOCK_INDENT_SIZE);
    switch (expr->operator_type)
    {
    case ceot_add:
    case ceot_sub:
    case ceot_mul:
    case ceot_div:
    case ceot_eq:
    case ceot_lt:
    case ceot_gt:
    case ceot_and:
    case ceot_or:
        printf("operator %c\n", (char)expr->operator_type);
        dump_expr(expr->variant.binary_op_childs.left_child, _scope_count+1);
        dump_expr(expr->variant.binary_op_childs.right_child, _scope_count+1);
        break;

    case ceot_factor:
        if (expr->variant.single_factor->factor_type == ceft_id)
            printf("entry-%d\n", expr->variant.single_factor->variant.id);
        else
            printf("number-%d\n", expr->variant.single_factor->variant.number);
        break;
    }
}

static void dump_statement_list(c_list *_stmt_list, int _scope_count)
{
    for (int i = 0; i < g_list_size(_stmt_list); i++)
    {
        dump_statement(g_list_get(_stmt_list, i), _scope_count);
    }
}

static void dump_statement(c_stmt *_stmt, int _scope_count)
{
    switch (_stmt->stmt_type)
    {
    case cst_print_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("print ");
        for (int i = 0;
             i < g_list_size(_stmt->variant.print_stmt->id_list);
             i++)
        {
            printf("entry-%d ",
                   *(int*)g_list_get(_stmt->variant.print_stmt->id_list, i));
        }
        putchar('\n');
        break;

    case cst_let_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("let\n");
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        print_space(STMT_INDENT_SIZE);
        printf("entry-%d :=\n", _stmt->variant.let_stmt->id);
        dump_expr(_stmt->variant.let_stmt->expr, _scope_count+1);
        putchar('\n');
        break;

    case cst_if_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("if\n");
        print_space((_scope_count+1) * BLOCK_INDENT_SIZE);
        printf("condition: expr\n");
        dump_expr(_stmt->variant.if_stmt->condition, _scope_count+1);
        print_space((_scope_count+1) * BLOCK_INDENT_SIZE);
        printf("execution: stmt\n");
        dump_statement(_stmt->variant.if_stmt->execution, _scope_count+1);
        putchar('\n');
        break;

    case cst_input_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("input ");
        for (int i = 0;
             i < g_list_size(_stmt->variant.print_stmt->id_list);
             i++)
        {
            printf("entry-%d ",
                   *(int*)g_list_get(_stmt->variant.print_stmt->id_list, i));
        }
        putchar('\n');
        break;

    case cst_flag_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("flag flag-%d\n", _stmt->variant.flag_stmt->id);
        break;

    case cst_goto_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("goto flag-%d\n", _stmt->variant.flag_stmt->id);
        break;

    case cst_var_stmt:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("var ");
        for (int i = 0;
             i < g_list_size(_stmt->variant.print_stmt->id_list);
             i++)
        {
            printf("entry-%d ",
                   *(int*)g_list_get(_stmt->variant.print_stmt->id_list, i));
        }
        putchar('\n');
        break;

    case cst_stmt_block:
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("block\n");
        dump_env(_stmt->variant.block->env, _scope_count+1);
        dump_statement_list(_stmt->variant.block->stmt_list, _scope_count+1);
        putchar('\n');
        break;
    }
}

static void dump_env(c_env *_env, int _scope_count)
{
    print_space(_scope_count * BLOCK_INDENT_SIZE);
    printf("env table : \n");
    for (int i = 0; i < g_list_size(_env->entries); i++)
    {
        c_env_entry *entry = g_list_get(_env->entries, i);
        print_space(_scope_count * BLOCK_INDENT_SIZE);
        printf("%s - %d\n", g_string_cstr(entry->str), entry->id);
    }
}

static void print_space(int _count)
{
    for (int i = 0; i < _count; i++) putchar(' ');
}
