#include "env.h"
#include <stdlib.h>

static int gi_get_next_id(void)
{
    static int static_id = 0;
    static_id++;
    return static_id;
}

static c_env_entry *gi_create_env_entry(c_string* _str, int _id)
{
    c_env_entry *ret = (c_env_entry*)malloc(sizeof(c_env_entry));

    ret->str = g_create_n_init_string(g_string_cstr(_str),
                                      &default_allocator,
                                      &default_deallocator);
    ret->id = _id;
    return ret;
}

static void entry_deleter(void* _entry)
{
    c_env_entry *entry = (c_env_entry*)_entry;
    g_delete_string(entry->str);
}

extern c_env *g_create_env(c_env* _parent)
{
    c_env *ret = (c_env*)malloc(sizeof(c_env));
    ret->parent = _parent;
    ret->entries = g_create_list(&entry_deleter,
                                 &default_allocator,
                                 &default_deallocator);
    return ret;
}

extern int g_env_push(c_env *_env, c_string *_str)
{
    c_env_entry *entry = gi_create_env_entry(_str, gi_get_next_id());

    g_list_push_back(_env->entries, entry);
    return entry->id;
}

extern int g_env_query(c_env *_env, c_string *_str)
{
    for (int i = 0; i < g_list_size(_env->entries); i++)
    {
        c_env_entry *entry = (c_env_entry*)g_list_get(_env->entries, i);
        if (!g_string_strcmp(entry->str, _str))
        {
            return entry->id;
        }
    }

    if (_env->parent == NULL) return -1;
    return g_env_query(_env->parent, _str);
}

extern void g_delete_env(c_env* _env)
{
    g_delete_list(_env->entries);
    free(_env);
}
