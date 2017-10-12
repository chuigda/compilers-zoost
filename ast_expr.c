#include "ast_expr.h"
#include "error.h"
#include <stdlib.h>

extern c_expr* g_parse_expr(c_list *_tokens, int *_index, c_env *_env)
{
    c_expr_operator *ret = (c_expr_operator*)malloc(sizeof(c_expr_operator));
    c_token *current_token = g_list_get(_tokens, *_index);

    switch (current_token->token_type)
    {
    case token_sym_add:
    case token_sym_sub:
    case token_sym_mul:
    case token_sym_div:
    case token_sym_lt:
    case token_sym_gt:
    case token_sym_eq:
    case token_sym_and:
    case token_sym_or:
        {
            ret->operator_type = current_token->token_type;
            (*_index)++;
            ret->variant.binary_op_childs.left_child =
                    g_parse_expr(_tokens, _index, _env);
            ret->variant.binary_op_childs.right_child =
                    g_parse_expr(_tokens, _index, _env);
            break;
        }

    case token_number:
        {
            ret->operator_type = ceot_factor;
            c_expr_factor *factor =
                    (c_expr_factor*)malloc(sizeof(c_expr_factor));
            factor->factor_type = ceft_number;
            factor->variant.number = current_token->value.number;
            ret->variant.single_factor = factor;
            (*_index)++;
            break;
        }

    case token_string:
        {
            ret->operator_type = ceot_factor;
            c_expr_factor *factor =
                    (c_expr_factor*)malloc(sizeof(c_expr_factor));
            factor->factor_type = ceft_id;
            factor->variant.id =
                    g_env_query(_env, current_token->value.string);
            if (factor->variant.id == -1)
            {
                error(current_token->line, "UNKNOWN IDENTIFIER");
                free(ret);
                return NULL;
            }
            ret->variant.single_factor = factor;
            (*_index)++;
            break;
        }

    default:
        error(current_token->line, "BAD EXPRESSION");
        free(ret);
        return NULL;
    }
    return ret;
}

extern void g_delete_expr(c_expr *_expr)
{
    switch (_expr->operator_type)
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
        {
            g_delete_expr(_expr->variant.binary_op_childs.left_child);
            g_delete_expr(_expr->variant.binary_op_childs.right_child);
            break;
        }

    case ceot_factor:
        {
            free(_expr->variant.single_factor);
            break;
        }
    }
    free(_expr);
}
