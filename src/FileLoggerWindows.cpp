#define _WIN32_WINNT 0x0500
#define NOGDI

#define INVALID_HANDLE_VALUE (HANDLE(LONG_PTR(-1)))
#define STD_INPUT_HANDLE (DWORD(-10))
#define STD_OUTPUT_HANDLE (DWORD(-11))
#define STD_ERROR_HANDLE (DWORD(-12))

#include <cstdio>
#include "common_windefines.h"
#include <debugapi.h>
#include <fileapi.h>
#include <windef.h>
#include <wincon.h>
#include <WinUser.h>
#include <ProcessEnv.h>

namespace ncine {

bool createConsole(bool hasTermEnv)
{
	FILE *fDummy = nullptr;

	if (AttachConsole(ATTACH_PARENT_PROCESS) && hasTermEnv == false)
	{
		const HANDLE consoleHandleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (consoleHandleOut != INVALID_HANDLE_VALUE)
		{
			freopen_s(&fDummy, "CONOUT$", "w", stdout);
			setvbuf(stdout, NULL, _IONBF, 0);
		}

		const HANDLE consoleHandleError = GetStdHandle(STD_ERROR_HANDLE);
		if (consoleHandleError != INVALID_HANDLE_VALUE)
		{
			freopen_s(&fDummy, "CONOUT$", "w", stderr);
			setvbuf(stderr, NULL, _IONBF, 0);
		}

		const HANDLE consoleHandleIn = GetStdHandle(STD_INPUT_HANDLE);
		if (consoleHandleIn != INVALID_HANDLE_VALUE)
		{
			freopen_s(&fDummy, "CONIN$", "r", stdin);
			setvbuf(stdin, NULL, _IONBF, 0);
		}

		return true;
	}
	else if (AllocConsole())
	{
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);

		HANDLE hConOut = CreateFile(TEXT("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hConIn = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
		SetStdHandle(STD_ERROR_HANDLE, hConOut);
		SetStdHandle(STD_INPUT_HANDLE, hConIn);

		return true;
	}

	return false;
}

void destroyConsole(bool hasTermEnv)
{
	// The "Enter" key is only sent if the console window is in focus
	if (GetConsoleWindow() == GetForegroundWindow() && hasTermEnv == false)
	{
		// Send the "Enter" key to the console to release the command prompt
		INPUT ip;

		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		ip.ki.wVk = 0x0D; // virtual-key code for the "Enter" key
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));

		ip.ki.dwFlags = KEYEVENTF_KEYUP; // `KEYEVENTF_KEYUP` for key release
		SendInput(1, &ip, sizeof(INPUT));
	}

	FreeConsole();
}

bool enableVirtualTerminalProcessing()
{
	const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
		return false;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
		return false;

	return true;
}

void writeOutputDebug(const char *logEntry)
{
	if (IsDebuggerPresent())
		OutputDebugString(logEntry);
}

}
