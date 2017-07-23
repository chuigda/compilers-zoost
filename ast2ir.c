#include "ast2ir.h"
#include "error.h"

#include <stdlib.h>

static void gi_translate_ast_block(
        c_list *ret, c_block *_block, c_flags_table *_flags,
        c_incomplete_goto_table *_gotos);

static void gi_translate_stmt(
        c_list *ret, c_stmt *_stmt, c_flags_table *_flags,
        c_incomplete_goto_table *_gotos);

static void gi_translate_var_stmt(c_list *ret, c_var_stmt *_stmt);

static void gi_translate_input_stmt(c_list *ret, c_input_stmt *_stmt);

static void gi_translate_print_stmt(c_list *ret, c_print_stmt *_stmt);

static void gi_translate_if_stmt(
        c_list *ret, c_if_stmt *_stmt, c_flags_table* _flags,
        c_incomplete_goto_table *_gotos);

static void gi_translate_let_stmt(c_list *ret, c_let_stmt *_stmt);

static void gi_translate_flag_stmt(
        c_list *ret, c_flag_stmt *_stmt, c_flags_table *_flags);

static void gi_translate_goto_stmt(
        c_list *ret, c_flag_stmt *_stmt, c_incomplete_goto_table *_gotos);


static void gi_finalize_goto_stmt(c_flags_table *_table,
                                  c_incomplete_goto_table *_gotos);

extern int gi_translate_expr_n_return(c_list *ret, c_expr* _expr, int*);

extern c_list* g_translate_ast(c_ast_translation_unit *_trans_unit)
{
    c_list *ret = g_create_list(&default_trivial_deleter,
                                &default_allocator,
                                &default_deallocator);

    c_flags_table *flags =
            (c_flags_table*)malloc(sizeof(c_flags_table));

    flags->flags = g_create_list(&default_trivial_deleter,
                                 &default_allocator,
                                 &default_deallocator);

    c_incomplete_goto_table *gotos =
            (c_incomplete_goto_table*)malloc(sizeof(c_incomplete_goto_table));
    gotos->goto_ir_instances = g_create_list(&default_trivial_deleter,
                                             &default_allocator,
                                             &default_deallocator);

    for (int i = 0; i < g_list_size(_trans_unit->statements); i++)
    {
        gi_translate_stmt(
            ret, g_list_get(_trans_unit->statements, i), flags, gotos);
    }

    for (int i = 0; i < g_list_size(_trans_unit->global_env->entries); i++)
    {
        c_ir_instance *deallocate_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));
        deallocate_instance->operation = cirop_deallocate;
        deallocate_instance->first.number =
            ((c_env_entry*)g_list_get(_trans_unit->global_env->entries, i))->id;

        g_list_push_back(ret, (void*)deallocate_instance);
    }

    c_ir_instance *endp_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));
    endp_instance->operation = cirop_endp;
    g_list_push_back(ret, (void*)endp_instance);

    gi_finalize_goto_stmt(flags, gotos);

    g_list_reset_lose_memory(gotos->goto_ir_instances);
    g_delete_list(flags->flags);
    g_delete_list(gotos->goto_ir_instances);

    free(flags);
    free(gotos);

    return ret;
}

static void gi_translate_ast_block(
        c_list *ret, c_block *_block, c_flags_table *_flags,
        c_incomplete_goto_table *_gotos)
{
    for (int i = 0; i < g_list_size(_block->stmt_list); i++)
    {
        gi_translate_stmt(
                    ret, g_list_get(_block->stmt_list, i), _flags, _gotos);
    }

    for (int i = 0; i < g_list_size(_block->env->entries); i++)
    {
        c_ir_instance *dealloc_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));
        dealloc_instance->operation = cirop_deallocate;
        dealloc_instance->first.number =
            ((c_env_entry*)g_list_get(_block->env->entries, i))->id;

        g_list_push_back(ret, (void*)dealloc_instance);
    }
}

static void gi_translate_stmt(
        c_list *ret, c_stmt *_stmt, c_flags_table *_flags,
        c_incomplete_goto_table *_gotos)
{
    switch (_stmt->stmt_type)
    {
    case cst_var_stmt:
        gi_translate_var_stmt(ret, _stmt->variant.var_stmt);
        break;

    case cst_input_stmt:
        gi_translate_input_stmt(ret, _stmt->variant.input_stmt);
        break;

    case cst_print_stmt:
        gi_translate_print_stmt(ret, _stmt->variant.print_stmt);
        break;

    case cst_if_stmt:
        gi_translate_if_stmt(ret, _stmt->variant.if_stmt, _flags, _gotos);
        break;

    case cst_let_stmt:
        gi_translate_let_stmt(ret, _stmt->variant.let_stmt);
        break;

    case cst_flag_stmt:
        gi_translate_flag_stmt(ret, _stmt->variant.flag_stmt, _flags);
        break;

    case cst_goto_stmt:
        gi_translate_goto_stmt(ret, _stmt->variant.goto_stmt, _gotos);
        break;

    case cst_stmt_block:
        gi_translate_ast_block(ret, _stmt->variant.block,  _flags, _gotos);
        break;

    default:
        error(0, THEFUCK);
    }
}

static void gi_translate_var_stmt(c_list *ret, c_var_stmt *_stmt)
{
    for (int i = 0; i < g_list_size(_stmt->id_list); i++)
    {
        c_ir_instance *alloc_instance =
                (c_ir_instance*)malloc(sizeof(c_ir_instance));

        alloc_instance->operation = cirop_allocate;
        alloc_instance->first.type = cirodt_index;
        alloc_instance->first.number = *(int*)g_list_get(_stmt->id_list, i);

        g_list_push_back(ret, alloc_instance);
    }
}

static void gi_translate_input_stmt(c_list *ret, c_input_stmt *_stmt)
{
    for (int i = 0; i < g_list_size(_stmt->id_list); i++)
    {
        c_ir_instance *alloc_instance =
                (c_ir_instance*)malloc(sizeof(c_ir_instance));

        alloc_instance->operation = cirop_input;
        alloc_instance->first.type = cirodt_index;
        alloc_instance->first.number = *(int*)g_list_get(_stmt->id_list, i);

        g_list_push_back(ret, alloc_instance);
    }
}

static void gi_translate_print_stmt(c_list *ret, c_input_stmt *_stmt)
{
    for (int i = 0; i < g_list_size(_stmt->id_list); i++)
    {
        c_ir_instance *alloc_instance =
                (c_ir_instance*)malloc(sizeof(c_ir_instance));

        alloc_instance->operation = cirop_print;
        alloc_instance->first.type = cirodt_index;
        alloc_instance->first.number = *(int*)g_list_get(_stmt->id_list, i);

        g_list_push_back(ret, alloc_instance);
    }
}

static void gi_translate_if_stmt(c_list *ret,
                                 c_if_stmt *_stmt,
                                 c_flags_table *_flags,
                                 c_incomplete_goto_table *_gotos)
{
    c_ir_instance *branch_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));


    int q = 0;
    branch_instance->operation = cirop_branchfalse;
    branch_instance->first.type = cirodt_number;
    branch_instance->second.type = cirodt_temp_index;
    branch_instance->second.number =
            gi_translate_expr_n_return(ret, _stmt->condition, &q);

    g_list_push_back(ret, (void*)branch_instance);
    gi_translate_stmt(ret, _stmt->execution, _flags, _gotos);
    branch_instance->first.number = g_list_size(ret);
}

static void gi_translate_let_stmt(c_list *ret, c_let_stmt *_stmt)
{
    c_ir_instance *assign_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));

    int q = 0;

    assign_instance->operation = cirop_assign;
    assign_instance->first.type = cirodt_index;
    assign_instance->first.number = _stmt->id;

    assign_instance->second.type = cirodt_temp_index;
    assign_instance->second.number =
            gi_translate_expr_n_return(ret, _stmt->expr, &q);

    g_list_push_back(ret, assign_instance);
}

static void gi_translate_flag_stmt(
        c_list *ret, c_flag_stmt *_stmt, c_flags_table *_flags)
{
    c_flag *new_flag = (c_flag*)malloc(sizeof(c_flag));
    new_flag->flag_id = _stmt->id;
    new_flag->flag_position = g_list_size(ret);

    g_list_push_back(_flags->flags, (void*)new_flag);
}

static void gi_translate_goto_stmt(c_list *ret, c_flag_stmt *_stmt,
                                   c_incomplete_goto_table *_gotos)
{
    c_ir_instance *goto_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));
    goto_instance->operation = cirop_branch;
    goto_instance->first.type = cirodt_index;
    goto_instance->first.number = _stmt->id;

    g_list_push_back(_gotos->goto_ir_instances, (void*)goto_instance);
    g_list_push_back(ret, (void*)goto_instance);
}

static void gi_finalize_goto_stmt(c_flags_table *_table,
                                  c_incomplete_goto_table *_gotos)
{
    for (int i = 0; i < g_list_size(_gotos->goto_ir_instances); i++)
    {
        c_ir_instance *goto_instance =
                (c_ir_instance*)g_list_get(_gotos->goto_ir_instances, i);

        int goto_resolved = 0;
        for (int j = 0; j < g_list_size(_table->flags); j++)
        {
            c_flag* flag = g_list_get(_table->flags, j);

            if (flag->flag_id == goto_instance->first.number)
            {
                goto_instance->first.type = cirodt_number;
                goto_instance->first.number = flag->flag_position;

                goto_resolved = 1;
                break;
            }

            if (!goto_resolved)
            {
                error(0, THEFUCK);
            }
        }
    }
}
