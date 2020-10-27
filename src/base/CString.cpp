#include <cstdio> // for vsnprintf()
#include <cstring>
#include <nctl/CString.h>

namespace nctl {

size_t strnlen(const char *str, size_t maxLen)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	return strnlen_s(str, maxLen);
#else
	return ::strnlen(str, maxLen);
#endif
}

char *strncpy(char *dest, const char *source, size_t count)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(dest, count + 1, source, count);
	return dest;
#else
	return ::strncpy(dest, source, count);
#endif
}

char *strncpy(char *dest, size_t elements, const char *source, size_t count)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(dest, elements, source, count);
	return dest;
#else
	return ::strncpy(dest, source, count);
#endif
}

char *strncat(char *dest, const char *source, size_t count)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	strncat_s(dest, count + 1, source, count);
	return dest;
#else
	return ::strncat(dest, source, count);
#endif
}

char *strncat(char *dest, size_t elements, const char *source, size_t count)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	strncat_s(dest, elements, source, count);
	return dest;
#else
	return ::strncat(dest, source, count);
#endif
}

int vsnprintf(char *str, size_t maxLen, const char *format, va_list arg)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	const int writtenChars = vsnprintf_s(str, maxLen, maxLen - 1, format, arg);
	return (writtenChars > -1) ? writtenChars : maxLen - 1;
#else
	return ::vsnprintf(str, maxLen, format, arg);
#endif
}

}
