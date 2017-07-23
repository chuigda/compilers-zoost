#include "ast2ir.h"
#include "error.h"

#include <stdlib.h>

extern int gi_translate_expr_n_return(
        c_list *ret, c_expr* _expr, int *temp_register_count)
{
    int ret_index;

    c_ir_instance *new_ir_instance =
            (c_ir_instance*)malloc(sizeof(c_ir_instance));

    if (_expr->operator_type == ceot_factor)
    {
        new_ir_instance->operation = cirop_assign;
        new_ir_instance->first.type = cirodt_temp_index;
        new_ir_instance->first.number = *temp_register_count;
        ret_index = *temp_register_count;
        (*temp_register_count)++;

        if (_expr->variant.single_factor->factor_type == ceft_id)
        {
            new_ir_instance->second.type = cirodt_index;
            new_ir_instance->second.number =
                    _expr->variant.single_factor->variant.id;
        }
        else
        {
            new_ir_instance->second.type = cirodt_number;
            new_ir_instance->second.number =
                    _expr->variant.single_factor->variant.number;
        }
    }
    else
    {
        new_ir_instance->operation = _expr->operator_type;

        new_ir_instance->first.type = cirodt_temp_index;
        new_ir_instance->first.number = *temp_register_count;
        ret_index = *temp_register_count;
        (*temp_register_count)++;

        new_ir_instance->second.type = cirodt_temp_index;
        new_ir_instance->third.type = cirodt_temp_index;

        new_ir_instance->second.number =
                gi_translate_expr_n_return(
                    ret, _expr->variant.binary_op_childs.left_child,
                    temp_register_count);

        new_ir_instance->third.number =
                gi_translate_expr_n_return(
                    ret, _expr->variant.binary_op_childs.right_child,
                    temp_register_count);
    }
    g_list_push_back(ret, (void*)new_ir_instance);

    return ret_index;
}
