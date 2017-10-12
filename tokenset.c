#include "tokenset.h"
#include "lex.h"

int g_in_set(c_token_set *_set, int _token)
{
    int i;
    for (i = 0; i < _set->count; ++i)
        if (_set->array[i] == _token)
            return 1;
    return 0;
}

c_token_set* g_setof_stmt_begin()
{
    static c_token_set set;
    static int init = 0;
    if (!init)
    {
        int i;
        for (i = 0; i < token_rwd_var; i++)
            set.array[i] = i;
        set.array[i+1] = token_sym_lbracket;
        set.array[i+2] = token_eoi;

        set.count = i+3;
    }
    return &set;
}
