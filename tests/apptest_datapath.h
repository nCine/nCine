#include "AppConfiguration.h"

inline void setDataPath(nc::AppConfiguration &config)
{
#ifdef __ANDROID__
	const char *extStorage = getenv("EXTERNAL_STORAGE");
	nctl::String dataPath;
	dataPath = extStorage ? extStorage : "/sdcard";
	dataPath += "/ncine/";
	config.setDataPath(dataPath);
#else
	#ifdef NCINE_TESTS_DATA_DIR
	config.setDataPath(NCINE_TESTS_DATA_DIR);
	#endif
#endif
}
