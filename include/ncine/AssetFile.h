#ifndef CLASS_NCINE_ASSETFILE
#define CLASS_NCINE_ASSETFILE

#include "IFile.h"
#include <android_native_app_glue.h> // for android_app
#include <android/asset_manager.h>

namespace ncine {

/// The class dealing with Android asset files
class DLL_PUBLIC AssetFile : public IFile
{
  public:
	/// Constructs an asset file object
	/*! \param filename File name including path relative to the assets directory */
	explicit AssetFile(const char *filename);
	~AssetFile() override;

	inline static FileType sType() { return FileType::ASSET; }

	/// Tries to open the asset file
	void open(unsigned char mode) override;
	/// Closes the asset file
	void close() override;
	long int seek(long int offset, int whence) const override;
	long int tell() const override;
	unsigned long int read(void *buffer, unsigned long int bytes) const override;
	unsigned long int write(void *buffer, unsigned long int bytes) override { return 0; }

	bool isOpened() const override;

	/// Sets the global pointer to the AAssetManager
	static void initAssetManager(struct android_app *state) { assetManager_ = state->activity->assetManager; }

	/// Checks if an asset file exists and can be opened
	static bool tryOpen(const char *filename);

  private:
	static AAssetManager *assetManager_;
	AAsset *asset_;
	long int startOffset_;

	/// Opens the file with `AAsset_openFileDescriptor()`
	void openFD(unsigned char mode);
	/// Opens the file with `AAssetManager_open()` only
	void openAsset(unsigned char mode);
};

}

#endif
