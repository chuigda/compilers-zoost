#ifndef AST2IR_H
#define AST2IR_H

#include "ast_tree.h"
#include "c_list.h"
#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

extern c_list *g_translate_ast(c_ast_translation_unit *_trans_unit);
extern void g_ir_dump(c_list *ir_instances);

#ifdef __cplusplus
}
#endif

#endif // AST2IR_H
