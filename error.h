#ifndef ERROR_H
#define ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

extern void error(int _line, const char* _str);
extern int error_count;

#ifdef __cplusplus
}
#endif

#define THEFUCK "I DON'T EVER KNOW WHAT'S WRONG, JUST GO AND FUCK YOURSELF."

#endif // ERROR_H
