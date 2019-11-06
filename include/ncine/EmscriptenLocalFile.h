#ifndef CLASS_NCINE_EMSCRIPTENLOCALFILE
#define CLASS_NCINE_EMSCRIPTENLOCALFILE

#include <nctl/String.h>
#include <nctl/UniquePtr.h>

namespace ncine {

/// The class dealing with opening and saving a local file on Emscripten
class DLL_PUBLIC EmscriptenLocalFile
{
  public:
	using LoadedCallbackType = void(const EmscriptenLocalFile &localFile, void *userData);

	EmscriptenLocalFile()
	    : fileSize_(0), filename_(MaxFilenameLength), loading_(false), loadedCallback_(nullptr) {}

	/// Opens a dialog in the browser to choose a file to load
	void load();
	/// Opens a filtered dialog in the browser to choose a file to load
	void load(const char *fileFilter);
	/// Saves a file from the browser to the local machine
	void save(const char *filename);
	/// Reads a certain amount of bytes from the file to a buffer
	/*! \return Number of bytes read */
	unsigned long int read(void *buffer, unsigned long int bytes) const;
	/// Writes a certain amount of bytes from a buffer to the file
	/*! \return Number of bytes written */
	unsigned long int write(void *buffer, unsigned long int bytes);

	/// Sets the callback to be invoked when loading is complete
	inline void setLoadedCallback(LoadedCallbackType *loadedCallback) { setLoadedCallback(loadedCallback, nullptr); }
	/// Sets the callback to be invoked when loading is complete, together with user data
	void setLoadedCallback(LoadedCallbackType *loadedCallback, void *userData);

	/// Returns a read-only pointer to the internal file buffer
	inline const char *data() const { return fileBuffer_.get(); }
	/// Returns the file size in bytes
	inline long int size() const { return fileSize_; }
	/// Returns the file name
	inline const char *filename() const { return filename_.data(); }
	/// Returns true if loading is in progress
	inline bool isLoading() const { return loading_; }

  private:
	/// Maximum number of characters for a file name
	static const unsigned int MaxFilenameLength = 256;

	/// The memory buffer that stores file contents
	nctl::UniquePtr<char[]> fileBuffer_;
	/// File size in bytes
	long int fileSize_;
	/// File name
	nctl::String filename_;
	/// The flag indicating if the JavaScript loading callback has not yet returned
	bool loading_;

	LoadedCallbackType *loadedCallback_;
	void *userData_;

	static void fileDataCallback(void *context, char *contentPointer, size_t contentSize, const char *fileName);
	static void loadingCallback(void *context);
	static void cannotLoadCallback(void *context);
};

}

#endif
