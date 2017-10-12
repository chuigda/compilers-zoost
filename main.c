#include "error.h"
#include "lex.h"
#include "ast_tree.h"
#include "dump_ast.h"
#include "ast2ir.h"
#include "ir_interpret.h"

#include <stdio.h>
#include <stdlib.h>

#define RELEASE

#ifdef RELEASE

int main(int _argc, char** _argv)
{
    if (_argc != 2)
    {
        printf("Usage : zoost [filename]\n");
        return 0;
    }

    FILE *fp = fopen(_argv[1], "r");
    if (fp == NULL)
    {
        printf("Unable to open file %s.\n", _argv[1]);
    }

    c_string *str_code = g_create_n_init_string("",
                                                &default_allocator,
                                                &default_deallocator);

    char ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        g_string_append(str_code, ch);
    }
    fclose(fp);

    c_list *tokens = g_scan_and_split(str_code);
    if (error_count != 0) abort();
    c_ast_translation_unit *ast = g_parse_tokens(tokens);
    if (error_count != 0) abort();
    c_list *ir_instances = g_translate_ast(ast);
    g_execute_ir_instance(ir_instances);

    g_delete_ast(ast);
    g_delete_list(tokens);
    g_delete_list(ir_instances);
    g_delete_string(str_code);

    return 0;
}

#else

const char* src =
"var a, b;\n"
"[\n"
"    var a, b, c;\n"
"    input a, b;\n"
"    let c [- a b];\n"
"    if [< c 0]\n"
"    [\n"
"        let c [+ a b];\n"
"        goto 12;\n"
"    ]\n"
"    print c;\n"
"    flag 12;\n"
"    print c;\n"
"]\n"
"input a, b;\n"
"let b [+ a b];\n"
"print b;\n";

int main()
{
    c_string *code = g_create_n_init_string(
                         src, &default_allocator, &default_deallocator);

    c_list *tokens = g_scan_and_split(code);
    c_ast_translation_unit *ast = g_parse_tokens(tokens);
    c_list *ir_instances = g_translate_ast(ast);
    dump_ast(ast);
    putchar('\n');
    g_ir_dump(ir_instances);
    putchar('\n');
    g_execute_ir_instance(ir_instances);
    g_delete_ast(ast);
    g_delete_list(tokens);
    g_delete_list(ir_instances);
    g_delete_string(code);

    return 0;
}

#endif
