#include "lex.h"
#include "error.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static c_token* create_string_token(c_string** _str, int _line)
{
    c_token *ret = (c_token*)malloc(sizeof(c_token));
    ret->token_type = token_string;
    ret->value.string = *_str;
    *_str = NULL;
    ret->line = _line;
    return ret;
}

static c_token* create_rwd_token(int _rword, int _line)
{
    c_token *ret = (c_token*)malloc(sizeof(c_token));
    ret->token_type = _rword;
    ret->line = _line;
    return ret;
}

static c_token* create_number_token(int _number, int _line)
{
    c_token *ret = (c_token*)malloc(sizeof(c_token));
    ret->token_type = token_number;
    ret->value.number = _number;
    ret->line = _line;
    return ret;
}

static c_token* create_symbol_token(int _symbol, int _line)
{
    c_token *ret = (c_token*)malloc(sizeof(c_token));
    ret->token_type = _symbol;
    ret->line = _line;
    return ret;
}

static int scan_number(const c_string* _src_code, int* _current_pos)
{
    int ret = 0;

    while ( isdigit(g_string_at(_src_code, *_current_pos)) )
    {
        ret *= 10;
        ret += g_string_at(_src_code, *_current_pos) - '0';
        (*_current_pos)++;
    }
    return ret;
}

static c_string* scan_string(const c_string* _src_code, int* _current_pos)
{
    c_string* ret =
            g_create_n_init_string(
                "", &default_allocator, &default_deallocator);

    while ( isalpha(g_string_at(_src_code, *_current_pos) ))
    {
        g_string_append(ret, g_string_at(_src_code, *_current_pos));
        (*_current_pos)++;
    }
    return ret;
}

static void skip_whitespace(const c_string* _src_code, int* _current_pos)
{
    while ( g_string_at(_src_code, *_current_pos) == ' '
            || g_string_at(_src_code, *_current_pos) == '\t'
            || g_string_at(_src_code, *_current_pos) == '\v'
            || g_string_at(_src_code, *_current_pos) == '\f')
    {
        (*_current_pos)++;
    }
}

static int identify_rword(const c_string* _str)
{
    c_string *str_rwd_input =
            g_create_n_init_string(
                "input", &default_allocator, &default_deallocator);
    c_string *str_rwd_print =
            g_create_n_init_string(
                "print", &default_allocator, &default_deallocator);
    c_string *str_rwd_goto =
            g_create_n_init_string(
                "goto", &default_allocator, &default_deallocator);
    c_string *str_rwd_flag =
            g_create_n_init_string(
                "flag", &default_allocator, &default_deallocator);
    c_string *str_rwd_if =
            g_create_n_init_string(
                "if", &default_allocator, &default_deallocator);
    c_string *str_rwd_let =
            g_create_n_init_string(
                "let", &default_allocator, &default_deallocator);
    c_string *str_rwd_var =
            g_create_n_init_string(
                "var", &default_allocator, &default_deallocator);

    int ret = -1;
    if (g_string_strcmp(_str, str_rwd_input) == 0) ret = token_rwd_input;
    else if (g_string_strcmp(_str, str_rwd_print) == 0) ret = token_rwd_print;
    else if (g_string_strcmp(_str, str_rwd_goto) == 0) ret = token_rwd_goto;
    else if (g_string_strcmp(_str, str_rwd_flag) == 0) ret = token_rwd_flag;
    else if (g_string_strcmp(_str, str_rwd_if) == 0) ret = token_rwd_if;
    else if (g_string_strcmp(_str, str_rwd_let) == 0) ret = token_rwd_let;
    else if (g_string_strcmp(_str, str_rwd_var) == 0) ret = token_rwd_var;

    g_delete_string(str_rwd_input);
    g_delete_string(str_rwd_print);
    g_delete_string(str_rwd_goto);
    g_delete_string(str_rwd_flag);
    g_delete_string(str_rwd_if);
    g_delete_string(str_rwd_let);
    g_delete_string(str_rwd_var);

    return ret;
}

void g_delete_token(c_token* _token)
{
    if (_token->token_type == token_string)
    {
        g_delete_string(_token->value.string);
    }

    free(_token);
}

static void token_deleter(void* _token)
{
    if (((c_token*)_token)->token_type == token_string)
    {
        g_delete_string(((c_token*)_token)->value.string);
    }
}

extern c_list* g_scan_and_split(const c_string *_src_code)
{
    c_list *ret = g_create_list(&token_deleter,
                                &default_allocator,
                                &default_deallocator);

    int current_pos = 0;
    int current_line = 1;
    while (current_pos < g_string_strlen(_src_code))
    {
        c_string* data_string = NULL;

        switch (g_string_at(_src_code, current_pos))
        {
        case ' ': case '\t': case '\v': case '\f':
            { skip_whitespace(_src_code, &current_pos); break; }

        case '\n': { current_pos++; current_line++; break; }

        case 'f': case 'g': case 'i': case 'l': case 'p': case 'v':
            {
                data_string = scan_string(_src_code, &current_pos);
                int rword = identify_rword(data_string);
                if (rword != -1)
                {
                    g_list_push_back(ret,
                                     (void*)create_rwd_token(
                                         rword, current_line));
                    g_delete_string(data_string);
                    data_string = NULL;
                    break;
                }

                goto common_identifier_tag;
            }

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'h':
        case 'j': case 'k': case 'm': case 'n': case 'o': case 'q':
        case 'r': case 's': case 't': case 'u': case 'w': case 'x':
        case 'y': case 'z':
            {
                data_string = scan_string(_src_code, &current_pos);
                common_identifier_tag:

                g_list_push_back(ret,
                                 (void*)create_string_token(
                                     &data_string, current_line));
                break;
            }

        case '+': case '-': case '*': case '/': case '[': case ']':
        case ',': case ';': case '<': case '>': case '=': case '&':
        case '|': case '!':
            {
                g_list_push_back(ret,
                                 (void*)create_symbol_token(
                                     g_string_at(_src_code, current_pos),
                                     current_line));
                current_pos++;
                break;
            }


        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '0':
            {
                int number = scan_number(_src_code, &current_pos);
                g_list_push_back(ret,
                                 (void*)create_number_token(
                                     number, current_line));
                break;
            }

        default:
            {
                error(current_line,
                      "I don't ever know what's wrong,"
                      " just go and fuck yourself.");
                current_pos++;
            }
        }
    }

    return ret;
}

void g_dump_token(const c_token *_token)
{
    switch (_token->token_type)
    {
    case token_rwd_input: printf("reserved word INPUT\n"); break;
    case token_rwd_print: printf("reserved word PRINT\n"); break;
    case token_rwd_goto:  printf("reserved word GOTO\n"); break;
    case token_rwd_flag:  printf("reserved word FLAG\n"); break;
    case token_rwd_if:    printf("reserved word IF\n"); break;
    case token_rwd_let:   printf("reserved word LET\n"); break;
    case token_rwd_var:   printf("reserved word VAR\n"); break;

    case token_sym_add: printf("symbol +\n"); break;
    case token_sym_sub: printf("symbol -\n"); break;
    case token_sym_mul: printf("symbol *\n"); break;
    case token_sym_div: printf("symbol /\n"); break;
    case token_sym_lbracket: printf("symbol [\n"); break;
    case token_sym_rbracket: printf("symbol ]\n"); break;
    case token_sym_comma: printf("symbol ,\n"); break;
    case token_sym_semicolon: printf("symbol ;\n"); break;
    case token_sym_lt: printf("symbol <\n"); break;
    case token_sym_gt: printf("symbol >\n"); break;
    case token_sym_eq: printf("symbol =\n"); break;
    case token_sym_and: printf("symbol &\n"); break;
    case token_sym_or: printf("symbol |\n"); break;
    case token_sym_not: printf("symbol !\n"); break;

    case token_number: printf("number %d\n", _token->value.number); break;
    case token_string:
        printf("string %s\n", g_string_cstr(_token->value.string)); break;

    default:
        error(_token->line, "I don't ever know what's wrong,"
                            " just go and fuck yourself");
    }
}

