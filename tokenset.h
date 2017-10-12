#ifndef TOKENSET_H
#define TOKENSET_H

#ifdef __cplusplus
extern "C" {
#endif

#define SET_MAX_SIZE 32

typedef struct
{
    int array[SET_MAX_SIZE];
    int count;
} c_token_set;

extern int g_in_set(c_token_set *_set, int _token);

extern c_token_set* g_setof_stmt_begin();

#ifdef __cplusplus
}
#endif

#endif // TOKENSET_H
