#ifndef C_STRING_H
#define C_STRING_H

#include "c_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

struct c_string_impl;

typedef struct
{
    struct c_string_impl* p_impl;
}
c_string;

extern c_string* g_create_string(c_allocator* _allocator,
                          c_deallocator* _deallocator);
extern c_string* g_create_n_init_string(const char* _init,
                                 c_allocator* _allocator,
                                 c_deallocator* _deallocator);
extern void g_delete_string(c_string* _str);
extern void g_string_deleter(void* _str);

extern void g_string_append(c_string* _str, char ch);
extern char g_string_at(const c_string* _str, int _index);
extern void g_string_strcat(c_string* _dest, const c_string* _src);
extern int g_string_strlen(const c_string* _str);
extern int g_string_strcmp(const c_string* _str1, const c_string* _str2);

extern const char* g_string_cstr(const c_string* _str);

#ifdef __cplusplus
}
#endif

#endif // C_STRING_H
