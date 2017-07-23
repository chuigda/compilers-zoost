#ifndef ENV_H
#define ENV_H

#include "c_string.h"
#include "c_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    c_string *str;
    int id;
} c_env_entry;

typedef struct env_struct
{
    struct env_struct* parent;
    c_list *entries;
} c_env;

extern c_env *g_create_env(c_env* _parent);
extern int g_env_push(c_env* _env, c_string* _str);
extern int g_env_query(c_env* _env, c_string* _str);
extern void g_delete_env(c_env* _env);

#ifdef __cplusplus
}
#endif

#endif // ENV_H
