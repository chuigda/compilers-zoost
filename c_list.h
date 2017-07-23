#ifndef C_LIST_H
#define C_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "c_memory.h"

struct c_list_impl;

typedef struct
{
    struct c_list_impl* p_impl;
}
c_list;

extern c_list* g_create_list(c_object_deleter* _deleter,
                           c_allocator* _allocator,
                           c_deallocator* _deallocator);
extern void g_delete_list(c_list *_list);
extern void g_list_reset_lose_memory(c_list *_list);

extern void* g_list_get(c_list*_list, int _index);
extern void g_list_push_back(c_list* _list, void* data);
extern int g_list_size(const c_list* _list);

#ifdef __cplusplus
}
#endif

#endif // C_LIST_H
