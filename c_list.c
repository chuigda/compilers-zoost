#include "c_list.h"
#include <stdlib.h>
#include <assert.h>

struct c_list_impl
{
    void **array;
    c_object_deleter* deleter;
    c_allocator* allocator;
    c_deallocator* deallocator;
    int array_size;
    int array_usage;
};

c_list* g_create_list(c_object_deleter *_deleter,
                      c_allocator *_allocator,
                      c_deallocator *_deallocator)
{
    c_list *ret = _allocator(sizeof(c_list));
    ret->p_impl = _allocator(sizeof(struct c_list_impl));

    ret->p_impl->array = NULL;
    ret->p_impl->array_size = 0;
    ret->p_impl->array_usage = 0;
    ret->p_impl->deleter = _deleter;
    ret->p_impl->allocator = _allocator;
    ret->p_impl->deallocator = _deallocator;

    return ret;
}

void g_delete_list(c_list *_list)
{
    for (int i = 0; i < _list->p_impl->array_usage; i++)
    {
        _list->p_impl->deleter(_list->p_impl->array[i]);
        _list->p_impl->deallocator(_list->p_impl->array[i]);
    }

    _list->p_impl->deallocator(_list->p_impl->array);

    c_deallocator *deallocator = _list->p_impl->deallocator;
    deallocator(_list->p_impl);
    deallocator(_list);
}

void g_list_reset_lose_memory(c_list *_list)
{
    _list->p_impl->deallocator(_list->p_impl->array);

    _list->p_impl->array = NULL;
    _list->p_impl->array_size = 0;
    _list->p_impl->array_usage = 0;
}

void* g_list_get(c_list *_list, int _index)
{
    assert(_index < _list->p_impl->array_size);
    return _list->p_impl->array[_index];
}

void g_list_push_back(c_list *_list, void *data)
{
    if (NULL == _list->p_impl->array)
    {
        assert(0 == _list->p_impl->array_size);
        assert(0 == _list->p_impl->array_usage);

        _list->p_impl->array = _list->p_impl->allocator(2 * sizeof(void*));
        _list->p_impl->array_size = 2;
    }
    else if (_list->p_impl->array_size == _list->p_impl->array_usage)
    {
        void** new_array =
                (void**)_list->p_impl->allocator(
                    2*_list->p_impl->array_size*sizeof(void*));

        for (int i = 0; i < _list->p_impl->array_usage; i++)
        {
            new_array[i] = _list->p_impl->array[i];
        }

        _list->p_impl->deallocator(_list->p_impl->array);
        _list->p_impl->array = new_array;

        _list->p_impl->array_size *= 2;
    }

    _list->p_impl->array[_list->p_impl->array_usage] = data;
    _list->p_impl->array_usage++;
}

int g_list_size(const c_list *_list)
{
    return _list->p_impl->array_usage;
}
