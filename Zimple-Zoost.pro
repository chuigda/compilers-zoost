TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    lex.c \
    c_list.c \
    c_memory.c \
    c_string.c \
    error.c \
    ast_tree.c \
    env.c \
    dump_ast.c \
    ast_expr.c \
    ast2ir.c \
    expr2ir.c \
    irdump.c \
    ir_interpret.c

HEADERS += \
    lex.h \
    c_list.h \
    c_string.h \
    c_memory.h \
    error.h \
    ast_tree.h \
    env.h \
    dump_ast.h \
    ast_expr.h \
    ast2ir.h \
    ir_interpret.h \
    ir.h

STATECHARTS +=

DISTFILES += \
    AST-Design.text
