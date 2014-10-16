#ifndef CLASS_NCASSETFILE
#define CLASS_NCASSETFILE

#include "ncIFile.h"
#include <android_native_app_glue.h> // for android_app
#include <android/asset_manager.h>

/// The class dealing with file open/close, its path and extension
class ncAssetFile: public ncIFile
{
  public:
	/// Constructs an asset file object
	/*! \param filename File name including path relative to the assets directory */
	ncAssetFile(const char *filename);
	~ncAssetFile();

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
	void openFd(unsigned char mode);
	// Opens the file with AAssetManager_open() only
	void openAsset(unsigned char mode);

	// Checks if a file can be accessed with specified mode
	static bool access(const char *filename, unsigned char mode);

	friend bool ncIFile::access(const char *filename, unsigned char mode);
};

#endif
