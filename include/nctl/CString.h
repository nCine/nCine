#ifndef CLASS_NCTL_CSTRING
#define CLASS_NCTL_CSTRING

#include <ncine/common_macros.h>

namespace nctl {

DLL_PUBLIC size_t strnlen(const char *str, size_t maxLen);
DLL_PUBLIC int stricmp(const char *str1, const char *str2);
DLL_PUBLIC char *strncpy(char *dest, const char *source, size_t count);
DLL_PUBLIC char *strncpy(char *dest, size_t elements, const char *source, size_t count);
DLL_PUBLIC char *strncat(char *dest, const char *source, size_t count);
DLL_PUBLIC char *strncat(char *dest, size_t elements, const char *source, size_t count);
DLL_PUBLIC int vsnprintf(char *str, size_t maxLen, const char *format, va_list arg);

}

#endif
