#include "ast2ir.h"

#include <stdio.h>

static void print_operand(c_ir_operand *_operand)
{
    switch (_operand->type)
    {
    case cirodt_index: printf("var"); break;
    case cirodt_number: break;
    case cirodt_temp_index: printf("t");break;
    }

    printf("%d", _operand->number);
}

extern void g_ir_dump(c_list *ir_instances)
{
    for (int i = 0; i < g_list_size(ir_instances); i++)
    {
        c_ir_instance *current_instance = g_list_get(ir_instances, i);

        switch (current_instance->operation)
        {
        case cirop_null:
            printf("nul");
            break;

        case cirop_allocate:
            printf("allocate %d", current_instance->first.number);
            break;

        case cirop_deallocate:
            printf("deallocate %d", current_instance->first.number);
            break;

        case cirop_assign:
            print_operand(&(current_instance->first));
            printf(" = ");
            print_operand(&(current_instance->second));
            break;

        case cirop_add_n_assign:
        case cirop_sub_n_assign:
        case cirop_mul_n_assign:
        case cirop_div_n_assign:
        case cirop_and_n_assign:
        case cirop_or_n_assign:
        case cirop_lt_n_assign:
        case cirop_gt_n_assign:
        case cirop_eq_n_assign:
            print_operand(&(current_instance->first));
            printf(" = ");
            print_operand(&(current_instance->second));
            printf(" %c ", (char)(current_instance->operation));
            print_operand(&(current_instance->third));
            break;

        case cirop_input:
            printf("input "); print_operand(&(current_instance->first)); break;
        case cirop_print:
            printf("print "); print_operand(&(current_instance->first)); break;

        case cirop_branch:
            printf("branch %d", current_instance->first.number);
            break;

        case cirop_branchfalse:
            printf("branch %d if ", current_instance->first.number);
            print_operand(&(current_instance->second));
            break;
        }

        putchar('\n');
    }
}
