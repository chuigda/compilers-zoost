#include "c_string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct c_string_impl
{
    char *buffer;
    int buffer_size;
    int string_length;

    c_allocator *allocator;
    c_deallocator *deallocator;
};

static void __internal_c_string_expand(c_string *_str, int _expected_size)
{
    if (_str->p_impl->buffer == NULL)
    {
        _str->p_impl->buffer =
                (char*)_str->p_impl->allocator(_expected_size*sizeof(char));
        _str->p_impl->buffer[0] = '\0';
        _str->p_impl->buffer_size = _expected_size;
        _str->p_impl->string_length = 0;
    }
    else if (_str->p_impl->buffer_size < _expected_size)
    {
        char *new_buffer =
                (char*)_str->p_impl->allocator(_expected_size*sizeof(char));
        strcpy(new_buffer, _str->p_impl->buffer);
        _str->p_impl->deallocator(_str->p_impl->buffer);
        _str->p_impl->buffer = new_buffer;
        _str->p_impl->buffer_size = _expected_size;
    }
}

c_string* g_create_string(c_allocator *_allocator,
                          c_deallocator *_deallocator)
{
    c_string* ret = (c_string*)_allocator(sizeof(c_string));
    ret->p_impl =
            (struct c_string_impl*)_allocator(sizeof(struct c_string_impl));
    ret->p_impl->buffer = NULL;
    ret->p_impl->buffer_size = 0;
    ret->p_impl->string_length = 0;
    ret->p_impl->allocator = _allocator;
    ret->p_impl->deallocator = _deallocator;
    return ret;
}

c_string* g_create_n_init_string(const char *_init,
                                 c_allocator *_allocator,
                                 c_deallocator *_deallocator)
{
    assert(_init);
    c_string *ret = g_create_string(_allocator, _deallocator);
    __internal_c_string_expand(ret, strlen(_init)+1);
    strcpy(ret->p_impl->buffer, _init);
    ret->p_impl->string_length = strlen(_init);
    return ret;
}

void g_delete_string(c_string *_str)
{
    c_deallocator *deallocator = _str->p_impl->deallocator;
    deallocator(_str->p_impl->buffer);
    deallocator(_str->p_impl);
    deallocator(_str);
}

void g_string_deleter(void *_str)
{
    c_string* str = (c_string*)_str;

    c_deallocator *deallocator = str->p_impl->deallocator;
    deallocator(str->p_impl->buffer);
    deallocator(str->p_impl);
}

void g_string_append(c_string *_str, char ch)
{
    __internal_c_string_expand(_str, _str->p_impl->string_length+2);
    _str->p_impl->buffer[_str->p_impl->string_length] = ch;
    _str->p_impl->string_length++;
    _str->p_impl->buffer[_str->p_impl->string_length] = '\0';
}

char g_string_at(const c_string *_str, int _index)
{
    return _str->p_impl->buffer[_index];
}

void g_string_strcat(c_string *_dest, const c_string *_src)
{
    __internal_c_string_expand(_dest,
                               _dest->p_impl->string_length
                               + _src->p_impl->string_length+2);

    strcat(_dest->p_impl->buffer, _src->p_impl->buffer);
}

int g_string_strlen(const c_string *_str)
{
    return _str->p_impl->string_length;
}

int g_string_strcmp(const c_string *_str1, const c_string *_str2)
{
    return strcmp(_str1->p_impl->buffer, _str2->p_impl->buffer);
}

const char* g_string_cstr(const c_string *_str)
{
    return _str->p_impl->buffer;
}
