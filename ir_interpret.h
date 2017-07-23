#ifndef IR_INTERPRET_H
#define IR_INTERPRET_H

#include "ir.h"
#include "c_list.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void g_execute_ir_instance(c_list *_ir_instances);

#ifdef __cplusplus
}
#endif

#endif // IR_INTERPRET_H
