#include "ir_interpret.h"
#include "error.h"

#include <stdio.h>

int* gi_bind_lvalue(int* _variables, int* _temps, c_ir_operand* _operand)
{
    switch (_operand->type)
    {
    case cirodt_index:
        return &_variables[_operand->number];

    case cirodt_temp_index:
        return &_temps[_operand->number];

    default:
        error(0, THEFUCK);
        return NULL;
    }
}

int gi_bind_rvalue(int* _variables, int* _temps, c_ir_operand* _operand)
{
    switch (_operand->type)
    {
    case cirodt_index:
        return _variables[_operand->number];

    case cirodt_number:
        return _operand->number;

    case cirodt_temp_index:
        return _temps[_operand->number];

    default:
        error(0, THEFUCK);
        return -1;
    }
}

extern void g_execute_ir_instance(c_list *_ir_instances)
{
    int ip = 0;

    int variables[4096];
    int temps[256];

    while (1)
    {
        c_ir_instance *current = g_list_get(_ir_instances, ip);

        switch (current->operation)
        {
        case cirop_null: break;
        case cirop_allocate: break;
        case cirop_deallocate: break;

        case cirop_assign:
            {
                int *left =
                    gi_bind_lvalue(variables, temps, &(current->first));
                int right =
                    gi_bind_rvalue(variables, temps, &(current->second));
                *left = right;
            break;
            }

        case cirop_add_n_assign:
        case cirop_sub_n_assign:
        case cirop_mul_n_assign:
        case cirop_div_n_assign:
        case cirop_and_n_assign:
        case cirop_or_n_assign:
        case cirop_lt_n_assign:
        case cirop_gt_n_assign:
        case cirop_eq_n_assign:
            {
                int *left =
                    gi_bind_lvalue(variables, temps, &(current->first));
                int operand1 =
                    gi_bind_rvalue(variables, temps, &(current->second));
                int operand2 =
                    gi_bind_rvalue(variables, temps, &(current->third));

                switch (current->operation)
                {
                case cirop_add_n_assign:
                    *left = operand1 + operand2; break;
                case cirop_sub_n_assign:
                    *left = operand1 - operand2; break;
                case cirop_mul_n_assign:
                    *left = operand1 * operand2; break;
                case cirop_div_n_assign:
                    *left = operand1 / operand2; break;
                case cirop_and_n_assign:
                    *left = operand1 && operand2; break;
                case cirop_or_n_assign:
                    *left = operand1 || operand2; break;
                case cirop_lt_n_assign:
                    *left = operand1 < operand2; break;
                case cirop_gt_n_assign:
                    *left = operand1 > operand2; break;
                case cirop_eq_n_assign:
                    *left = operand1 == operand2; break;
                }
            break;
            }

        case cirop_input:
            {
                printf("? ");
                scanf("%d",
                      gi_bind_lvalue(variables, temps, &(current->first)));
                fflush(stdin);
                break;
            }

        case cirop_print:
            {
                printf("%d\n",
                       gi_bind_rvalue(variables, temps, &(current->first)));
                fflush(stdout);
                break;
            }

        case cirop_branch:
            {
                ip = current->first.number;
                goto finish_branch;
            }

        case cirop_branchfalse:
            {
                if (!gi_bind_rvalue(variables, temps, &(current->second)))
                {
                    ip = current->first.number;
                    goto finish_branch;
                }
                break;
            }

        case cirop_endp:
            {
                return;
            }
        }

        ip++;

        finish_branch:
            {;}
    }
}
