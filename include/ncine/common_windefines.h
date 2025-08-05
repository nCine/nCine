#ifndef NCINE_COMMON_WINDEFINES
#define NCINE_COMMON_WINDEFINES

#if defined(_WIN32)
	#if !defined(_X86_) && !defined(_AMD64_) && defined(_M_IX86)
		#define _X86_
	#endif

	#if !defined(_X86_) && !defined(_AMD64_) && defined(_M_AMD64)
		#define _AMD64_
	#endif

	#define WIN32_LEAN_AND_MEAN
	#if !defined(NOMINMAX)
		#define NOMINMAX
	#endif

	#define _WIN32_WINNT 0x0602 // Windows 8 or newer (needed by `UserSemaphore`)
#endif

#endif
