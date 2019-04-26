#ifndef NCINE_COMMON_DEFINES
#define NCINE_COMMON_DEFINES

#ifndef NCINE_STATIC
	#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__) || defined(__CYGWIN__)
		#ifdef ncine_EXPORTS // automatically defined by CMake
			#if defined(__GNUC__) || defined(__llvm__)
				#define DLL_PUBLIC __attribute__((dllexport))
			#else
				#define DLL_PUBLIC __declspec(dllexport)
			#endif
		#else
			#if defined(__GNUC__) || defined(__llvm__)
				#define DLL_PUBLIC __attribute__((dllimport))
			#else
				#define DLL_PUBLIC __declspec(dllimport)
			#endif
		#endif
		#define DLL_LOCAL
	#else
		#if __GNUC__ >= 4 || defined(__llvm__)
			#define DLL_PUBLIC __attribute__((visibility("default")))
			#define DLL_LOCAL __attribute__((visibility("hidden")))
		#else
			#define DLL_PUBLIC
			#define DLL_LOCAL
		#endif
	#endif
#else
	#define DLL_PUBLIC
	#define DLL_LOCAL
#endif

#endif
