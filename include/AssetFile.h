#ifndef CLASS_NCINE_ASSETFILE
#define CLASS_NCINE_ASSETFILE

#include "IFile.h"
#include <android_native_app_glue.h> // for android_app
#include <android/asset_manager.h>

namespace ncine {

/// The class dealing with file open/close, its path and extension
class AssetFile: public IFile
{
  public:
	/// Constructs an asset file object
	/*! \param filename File name including path relative to the assets directory */
	AssetFile(const char *filename);
	~AssetFile();

	/// Static method to return class type
	inline static FileType sType() { return ASSET_TYPE; }

	void open(unsigned char mode);
	void close();
	long int seek(long int offset, int whence) const;
	long int tell() const;
	long int read(void *buffer, int bytes) const;

	bool isOpened() const;

	/// Sets the global pointer to the AAssetManager
	static void initAssetManager(struct android_app* state) { assetManager_ = state->activity->assetManager; }

  private:
	static AAssetManager* assetManager_;
	AAsset *asset_;
	long int startOffset_;

	// Opens the file with AAsset_openFileDescriptor()
	void openFD(unsigned char mode);
	// Opens the file with AAssetManager_open() only
	void openAsset(unsigned char mode);

	// Checks if a file can be accessed with specified mode
	static bool access(const char *filename, unsigned char mode);

	friend bool IFile::access(const char *filename, unsigned char mode);
};

}

#endif
