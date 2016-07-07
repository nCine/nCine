#ifndef CLASS_NCINE_ASSETFILE
#define CLASS_NCINE_ASSETFILE

#include "IFile.h"
#include <android_native_app_glue.h> // for android_app
#include <android/asset_manager.h>

namespace ncine {

/// The class dealing with Android asset files
class AssetFile: public IFile
{
  public:
	/// Constructs an asset file object
	/*! \param filename File name including path relative to the assets directory */
	explicit AssetFile(const char *filename);
	~AssetFile();

	inline static FileType sType() { return ASSET_TYPE; }

	/// Tries to open the asset file
	void open(unsigned char mode);
	/// Closes the asset file
	void close();
	long int seek(long int offset, int whence) const;
	long int tell() const;
	unsigned long int read(void *buffer, unsigned long int bytes) const;

	bool isOpened() const;

	/// Sets the global pointer to the AAssetManager
	static void initAssetManager(struct android_app* state) { assetManager_ = state->activity->assetManager; }

  private:
	static AAssetManager* assetManager_;
	AAsset *asset_;
	long int startOffset_;

	/// Opens the file with `AAsset_openFileDescriptor()`
	void openFD(unsigned char mode);
	/// Opens the file with `AAssetManager_open()` only
	void openAsset(unsigned char mode);

	/// Checks if a file can be accessed with specified mode
	static bool access(const char *filename, unsigned char mode);

	friend bool IFile::access(const char *filename, unsigned char mode);
};

}

#endif
