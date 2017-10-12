#include "ast_tree.h"
#include "lex.h"
#include "error.h"
#include "tokenset.h"

#include <stdlib.h>
#include <assert.h>

static void c_stmt_deleter(void* _deleted);
static void g_delete_stmt(c_stmt* _stmt);

static c_var_stmt* gi_parse_var_stmt(
        c_list *_tokens, int* _index, c_env *_env);

static c_input_stmt* gi_parse_input_stmt(
        c_list *_tokens, int *_index, c_env *_env);

static c_print_stmt* gi_parse_print_stmt(
        c_list *_tokens, int *_index, c_env *_env);

static c_flag_stmt* gi_parse_flag_stmt(
        c_list *_tokens, int *_index, c_env *_env);

static c_goto_stmt* gi_parse_goto_stmt(
        c_list *_tokens, int *_index, c_env *_env);

static c_let_stmt* gi_parse_let_stmt(
        c_list *_tokens, int *_index, c_env *_env);

static c_if_stmt* gi_parse_if_stmt(
        c_list *_tokens, int *_index, c_env *_env);

static c_block* gi_parse_block(
        c_list *_tokens, int *_index, c_env *_env);

static c_stmt* gi_parse_stmt(
        c_list* _tokens, int *_index, c_env* _env);

static void g_expect(
        c_list* _tokens, int *_index, int _expected);

static void gi_skip_to(c_list* _tokens, int *_index, c_token_set *_set);


c_ast_translation_unit *g_parse_tokens(c_list *_tokens)
{
    c_ast_translation_unit *ret =
            (c_ast_translation_unit*)malloc(sizeof(c_ast_translation_unit));
    ret->statements = g_create_list(&c_stmt_deleter,
                                    &default_allocator,
                                    &default_deallocator);
    ret->global_env = g_create_env(NULL);
    int current_index = 0;
    while (
        current_index < g_list_size(_tokens) &&
        ((c_token*)g_list_get(_tokens, current_index))->token_type!=token_eoi)
    {
        g_list_push_back(ret->statements,
                         (void*)gi_parse_stmt(
                             _tokens, &current_index, ret->global_env));
    }
    return ret;
}

static c_stmt* gi_parse_stmt(c_list *_tokens, int *_index, c_env *_env)
{
    c_stmt *ret = (c_stmt*)malloc(sizeof(c_stmt));
    c_token *current_token = (c_token*)g_list_get(_tokens, *_index);

    switch (current_token->token_type)
    {
    case token_rwd_var:
        ret->stmt_type = cst_var_stmt;
        (*_index)++;
        ret->variant.var_stmt = gi_parse_var_stmt(_tokens, _index, _env);
        break;

    case token_rwd_let:
        ret->stmt_type = cst_let_stmt;
        (*_index)++;
        ret->variant.let_stmt = gi_parse_let_stmt(_tokens, _index, _env);
        break;

    case token_rwd_if:
        ret->stmt_type = cst_if_stmt;
        (*_index)++;
        ret->variant.if_stmt = gi_parse_if_stmt(_tokens, _index, _env);
        break;

    case token_sym_lbracket:
        ret->stmt_type = cst_stmt_block;
        (*_index)++;
        ret->variant.block = gi_parse_block(_tokens, _index, _env);
        break;

    case token_sym_semicolon:
        ret->stmt_type = cst_invalid;
        (*_index)++;
        break;

    case token_rwd_input:
        ret->stmt_type = cst_input_stmt;
        (*_index)++;
        ret->variant.input_stmt = gi_parse_input_stmt(_tokens, _index, _env);
        break;

    case token_rwd_print:
        ret->stmt_type = cst_print_stmt;
        (*_index)++;
        ret->variant.input_stmt = gi_parse_print_stmt(_tokens, _index, _env);
        break;

    case token_rwd_flag:
        ret->stmt_type = cst_flag_stmt;
        (*_index)++;
        ret->variant.flag_stmt = gi_parse_flag_stmt(_tokens, _index, _env);
        break;

    case token_rwd_goto:
        ret->stmt_type = cst_goto_stmt;
        (*_index)++;
        ret->variant.goto_stmt = gi_parse_goto_stmt(_tokens, _index, _env);
        break;

    case token_eoi:
        break;

    default:
        error(current_token->line, "INVALID SYNTAX AS COMMENCE OF A STMT.");
        gi_skip_to(_tokens, _index, g_setof_stmt_begin());
    }

    return ret;
}

static c_var_stmt *gi_parse_var_stmt(c_list *_tokens, int *_index, c_env *_env)
{
    c_var_stmt *ret = (c_var_stmt*)malloc(sizeof(c_var_stmt));
    ret->id_list = g_create_list(&default_trivial_deleter,
                                 &default_allocator,
                                 &default_deallocator);

    c_token *current_token = (c_token*)g_list_get(_tokens, *_index);

    while (*_index < g_list_size(_tokens))
    {
        if (current_token->token_type != token_string)
        {
            error(current_token->line, "EXPECTED IDENTIFIER.");
            gi_skip_to(_tokens, _index, g_setof_stmt_begin());
            g_delete_list(ret->id_list);
            free(ret);
            return NULL;
        }

        int *new_id = (int*)malloc(sizeof(int));
        *new_id = g_env_push(_env, current_token->value.string);
        g_list_push_back(ret->id_list, (void*)new_id);
        (*_index)++;
        new_id = NULL;

        current_token = (c_token*)g_list_get(_tokens, *_index);

        if (current_token->token_type == token_sym_semicolon)
        {
            (*_index)++;
            return ret;
        }

        g_expect(_tokens, _index, token_sym_comma);
        current_token = (c_token*)g_list_get(_tokens, *_index);
    }

    error(current_token->line, THEFUCK);
    return NULL;
}

static c_let_stmt *gi_parse_let_stmt(c_list *_tokens, int *_index, c_env *_env)
{
    c_let_stmt *ret = (c_let_stmt*)malloc(sizeof(c_let_stmt));

    c_token *current_token = g_list_get(_tokens, *_index);
    if (current_token->token_type == token_string)
    {
        ret->id = g_env_query(_env, current_token->value.string);
        if (ret->id == -1) error(current_token->line, "UNKNOWN IDENTIFIER");
    }
    else
    {
        error(current_token->line, "EXPECTED IDENTIFIER");
        gi_skip_to(_tokens, _index, g_setof_stmt_begin());
    }
    (*_index)++;
    g_expect(_tokens, _index, token_sym_lbracket);
    ret->expr = g_parse_expr(_tokens, _index, _env);
    g_expect(_tokens, _index, token_sym_rbracket);
    g_expect(_tokens, _index, token_sym_semicolon);
    return ret;
}

static c_if_stmt *gi_parse_if_stmt(c_list *_tokens, int *_index, c_env *_env)
{
    c_if_stmt *ret = (c_if_stmt*)malloc(sizeof(c_if_stmt));

    g_expect(_tokens, _index, token_sym_lbracket);
    ret->condition = g_parse_expr(_tokens, _index, _env);
    g_expect(_tokens, _index, token_sym_rbracket);
    ret->execution = gi_parse_stmt(_tokens, _index, _env);
    return ret;
}

static c_input_stmt* gi_parse_input_stmt(
        c_list *_tokens, int *_index, c_env *_env)
{
    c_input_stmt *ret = (c_input_stmt*)malloc(sizeof(c_input_stmt));
    ret->id_list = g_create_list(&default_trivial_deleter,
                                 &default_allocator,
                                 &default_deallocator);

    c_token *current_token = (c_token*)g_list_get(_tokens, *_index);

    while (*_index < g_list_size(_tokens))
    {
        if (current_token->token_type != token_string)
        {
            error(current_token->line, "EXPECTED IDENTIFIER");
            (*_index)++;
            current_token = (c_token*)g_list_get(_tokens, *_index);
            continue;
        }

        int *new_id = (int*)malloc(sizeof(int));
        *new_id = g_env_query(_env, current_token->value.string);
        if (*new_id != -1)
        {
            g_list_push_back(ret->id_list, (void*)new_id);
        }
        else
        {
            error(current_token->line, "UNKNOWN IDENTIFIER");
            free(new_id);
        }
        (*_index)++;
        new_id = NULL;

        current_token = (c_token*)g_list_get(_tokens, *_index);

        if (current_token->token_type == token_sym_semicolon)
        {
            (*_index)++;
            return ret;
        }

        g_expect(_tokens, _index, token_sym_comma);
        current_token = (c_token*)g_list_get(_tokens, *_index);
    }

    g_delete_list(ret->id_list);
    free(ret);
    return NULL;
}

static c_print_stmt* gi_parse_print_stmt(
        c_list *_tokens, int *_index, c_env *_env)
{
    return (c_print_stmt*)gi_parse_input_stmt(_tokens, _index, _env);
}

static c_flag_stmt* gi_parse_flag_stmt(
        c_list *_tokens, int *_index, c_env *_env)
{
    (void)_env;

    c_flag_stmt* ret = (c_flag_stmt*)malloc(sizeof(c_flag_stmt));
    c_token* current_token = g_list_get(_tokens, *_index);
    if (current_token->token_type == token_number)
    {
        ret->id = current_token->value.number;
    }
    else
    {
        error(current_token->line, "EXPECTED A NUMBER AS LABEL");
    }
    (*_index)++;
    g_expect(_tokens, _index, token_sym_semicolon);
    return ret;
}

static c_goto_stmt* gi_parse_goto_stmt(
        c_list *_tokens, int *_index, c_env *_env)
{
    return (c_goto_stmt*)gi_parse_flag_stmt(_tokens, _index, _env);
}

static c_block* gi_parse_block(c_list *_tokens, int *_index, c_env *_env)
{
    c_block *ret = (c_block*)malloc(sizeof(c_block));
    ret->stmt_list = g_create_list(&c_stmt_deleter,
                                   &default_allocator,
                                   &default_deallocator);
    ret->env = g_create_env(_env);

    c_token *current_token = g_list_get(_tokens, *_index);
    while (current_token->token_type != ']'
           && current_token->token_type != token_eoi
           && *_index < g_list_size(_tokens))
    {
        g_list_push_back(ret->stmt_list,
                         (void*)gi_parse_stmt(_tokens, _index, ret->env));
        current_token = g_list_get(_tokens, *_index);
    }

    if (g_list_size(_tokens) == *_index)
    {
        error(((c_token*)g_list_get(_tokens, (*_index)-1))->line,
              "UNEXPECTED EOF");
        return 0;
    }
    g_expect(_tokens, _index, token_sym_rbracket);

    return ret;
}

static void g_expect(c_list *_tokens, int *_index, int _expected)
{
    if (((c_token*)g_list_get(_tokens, *_index))->token_type != _expected)
    {
        error(((c_token*)g_list_get(_tokens, *_index))->line,
              "UNEXPECTED TOKEN");
    }
    (*_index)++;
}

static void c_stmt_deleter(void *_deleted)
{
    c_stmt *stmt = (c_stmt*)_deleted;

    switch (stmt->stmt_type)
    {
    case cst_invalid:
        break;

    case cst_var_stmt:
        g_delete_list(stmt->variant.var_stmt->id_list);
        free(stmt->variant.var_stmt);
        break;

    case cst_input_stmt:
        g_delete_list(stmt->variant.input_stmt->id_list);
        free(stmt->variant.input_stmt);
        break;

    case cst_print_stmt:
        g_delete_list(stmt->variant.print_stmt->id_list);
        free(stmt->variant.print_stmt);
        break;

    case cst_let_stmt:
        g_delete_expr(stmt->variant.let_stmt->expr);
        free(stmt->variant.let_stmt);
        break;

    case cst_if_stmt:
        g_delete_expr(stmt->variant.if_stmt->condition);
        g_delete_stmt(stmt->variant.if_stmt->execution);
        free(stmt->variant.if_stmt);
        break;

    case cst_flag_stmt:
        free(stmt->variant.flag_stmt);
        break;

    case cst_goto_stmt:
        free(stmt->variant.goto_stmt);
        break;

    case cst_stmt_block:
        g_delete_env(stmt->variant.block->env);
        g_delete_list(stmt->variant.block->stmt_list);
        free(stmt->variant.block);
        break;

    default:
        error(0, THEFUCK);
        break;
    }
}

extern void g_delete_ast(c_ast_translation_unit *_ast)
{
    g_delete_env(_ast->global_env);
    g_delete_list(_ast->statements);

    free(_ast);
}

static void g_delete_stmt(c_stmt *_stmt)
{
    c_stmt_deleter(_stmt);
    free(_stmt);
}

static void gi_skip_to(c_list *_tokens, int *_index, c_token_set* _set)
{
    while (
       !g_in_set(_set,((c_token*)g_list_get(_tokens, *_index))->token_type))
        (*_index)++;
}

