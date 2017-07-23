#include "c_memory.h"

#include <stdlib.h>

void default_trivial_deleter(void* _unused) { (void)_unused; }
void* default_allocator(int _size) { return malloc(_size); }
void default_deallocator(void* _data) { free(_data); }
