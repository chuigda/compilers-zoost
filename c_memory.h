#ifndef C_MEMORY_H
#define C_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (c_object_deleter)(void*);
typedef void* (c_allocator)(int);
typedef void (c_deallocator)(void*);

extern void default_trivial_deleter(void*);
extern void* default_allocator(int);
extern void default_deallocator(void*);

#ifdef __cplusplus
}
#endif

#endif // C_MEMORY_H
