#ifndef IR_H
#define IR_H

#include "c_list.h"

typedef struct {
    int flag_id;
    int flag_position;
} c_flag;

typedef struct {
    c_list *flags;
} c_flags_table;

enum c_ir_operation_type
{
    cirop_null,

    cirop_allocate,
    cirop_deallocate,

    cirop_assign,

    cirop_add_n_assign = '+',
    cirop_sub_n_assign = '-',
    cirop_mul_n_assign = '*',
    cirop_div_n_assign = '/',

    cirop_and_n_assign = '&',
    cirop_or_n_assign = '|',
    cirop_lt_n_assign = '<',
    cirop_gt_n_assign = '>',
    cirop_eq_n_assign = '=',

    cirop_input = 896,
    cirop_print,

    cirop_branch,
    cirop_branchfalse,

    cirop_endp
};

enum c_ir_operand_type
{
    cirodt_number,
    cirodt_index,
    cirodt_temp_index
};

typedef struct
{
    int number;
    char type;
} c_ir_operand;

typedef struct
{
    unsigned operation;

    c_ir_operand first;
    c_ir_operand second;
    c_ir_operand third;
} c_ir_instance;

typedef struct
{
    c_list *goto_ir_instances;
} c_incomplete_goto_table;


#endif // IR_H
