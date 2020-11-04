#ifndef NCINE_RETURN_MACROS
#define NCINE_RETURN_MACROS

// Return assert macros
#define RETURN_ASSERT_MSG_X(x, fmt, ...) \
	do \
	{ \
		if (!(x)) \
		{ \
			LOGE_X(fmt, ##__VA_ARGS__); \
			return; \
		} \
	} while (false)

#define RETURN_ASSERT_MSG(x, fmt) \
	do \
	{ \
		if (!(x)) \
		{ \
			LOGE(fmt); \
			return; \
		} \
	} while (false)

#define RETURN_ASSERT(x) \
	do \
	{ \
		if (!(x)) \
		{ \
			LOGE("RETURN_ASSERT(" #x ")"); \
			return; \
		} \
	} while (false)

// Return macros
#define RETURN_MSG_X(fmt, ...) \
	do \
	{ \
		LOGE_X(fmt, ##__VA_ARGS__); \
		return; \
	} while (false)

#define RETURN_MSG(fmt) \
	do \
	{ \
		LOGE(fmt); \
		return; \
	} while (false)

// Return false assert macros
#define RETURNF_ASSERT_MSG_X(x, fmt, ...) \
	do \
	{ \
		if (!(x)) \
		{ \
			LOGE_X(fmt, ##__VA_ARGS__); \
			return false; \
		} \
	} while (false)

#define RETURNF_ASSERT_MSG(x, fmt) \
	do \
	{ \
		if (!(x)) \
		{ \
			LOGE(fmt); \
			return false; \
		} \
	} while (false)

#define RETURNF_ASSERT(x) \
	do \
	{ \
		if (!(x)) \
		{ \
			LOGE("RETURNF_ASSERT(" #x ")"); \
			return false; \
		} \
	} while (false)

// Return false macros
#define RETURNF_MSG_X(fmt, ...) \
	do \
	{ \
		LOGE_X(fmt, ##__VA_ARGS__); \
		return false; \
	} while (false)

#define RETURNF_MSG(fmt) \
	do \
	{ \
		LOGE(fmt); \
		return false; \
	} while (false)

#endif
