#define _WIN32_WINNT 0x0400
#include <Windows.h>

namespace ncine {

void writeOutputDebug(const char *logEntry)
{
	if (IsDebuggerPresent())
		OutputDebugString(logEntry);
}

}
