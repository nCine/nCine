#include <ncine/AppConfiguration.h>
#include <ncine/FileSystem.h>

inline void setDataPath(ncine::AppConfiguration &config)
{
#if defined(__ANDROID__)
	nctl::String dataPath = ncine::fs::externalStorageDir();
	dataPath += "/ncine/";
	config.dataPath() = dataPath;
#elif !defined(__EMSCRIPTEN__)
	#ifdef NCINE_TESTS_DATA_DIR
	config.dataPath() = NCINE_TESTS_DATA_DIR;
	#endif
#endif
}

inline nctl::String prefixDataPath(const char *directory, const char *file)
{
	nctl::String partialPath = ncine::fs::joinPath(directory, file);
	return ncine::fs::joinPath(ncine::fs::dataPath(), partialPath.data());
}
