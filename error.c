#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void error(int _line, const char *_str)
{
    fprintf(stderr, "At line %d : %s\n", _line, _str);
    fflush(stdout);
    abort();
}
