#include <ncine/AppConfiguration.h>
#include <ncine/FileSystem.h>

#if defined(__ANDROID__)
	#include <ncine/AssetFile.h>
#endif

inline void setDataPath(ncine::AppConfiguration &config)
{
#if defined(__ANDROID__)
	config.dataPath() = nc::AssetFile::Prefix;
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
