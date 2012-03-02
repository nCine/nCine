#ifndef CLASS_NCASSETFILE
#define CLASS_NCASSETFILE

#include "ncIFile.h"
#include <android_native_app_glue.h> // for android_app
#include <android/asset_manager.h>

/// The class dealing with file open/close, its path and extension
class ncAssetFile: public ncIFile
{
private:
	static AAssetManager* m_pAssetManager;
	AAsset *m_pAsset;
	long int m_lStartOffset;

	// Opens the file with AAsset_openFileDescriptor()
	void OpenFD(unsigned char uMode);
	// Opens the file with AAssetManager_open() only
	void OpenAsset(unsigned char uMode);

public:
	/// Constructs an asset file object
	/*! \param pFilename File name including path relative to the assets directory */
	ncAssetFile(const char *pFilename);
	~ncAssetFile()
	{
		if (m_bShouldCloseOnExit)
			Close();
	}

	/// Static method to return class type
	inline static eFileType sType() { return ASSET_TYPE; }

	void Open(unsigned char uMode);
	void Close();
	long int Seek(long int lOffset, int iWhence) const;
	long int Tell() const;
	long int Read(void *pBuffer, int iBytes) const;

	bool IsOpened() const;

	/// Sets the global pointer to the AAssetManager
	static void InitAssetManager(struct android_app* state) { m_pAssetManager = state->activity->assetManager; }
};

#endif
