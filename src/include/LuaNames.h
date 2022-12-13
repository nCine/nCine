#ifndef CLASS_NCINE_LUANAMES
#define CLASS_NCINE_LUANAMES

namespace ncine {

#if (defined __GNUC__ || defined __clang__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace LuaNames {
	static const char *ncine = "ncine";

	static const char *newObject = "new";
	static const char *deleteObject = "delete";
	static const char *cloneNode = "clone";

	static const char *input = "input";
}

#if (defined __GNUC__ || defined __clang__)
	#pragma GCC diagnostic pop
#endif

}

#endif
