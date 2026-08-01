#ifndef CLASS_NCINE_IIMAGESAVER
#define CLASS_NCINE_IIMAGESAVER

#include "common_defines.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class IFile;

/// Image saver interface class
class DLL_PUBLIC IImageSaver
{
  public:
	enum class Format
	{
		R8,
		RG8,
		RGB8,
		RGBA8,
		RGB_FLOAT
	};

	struct Properties
	{
		Properties()
		    : width(0), height(0), format(Format::RGB8), verticalFlip(false), pixels(nullptr) {}

		int width;
		int height;
		Format format;
		bool verticalFlip;
		void *pixels;
	};

	virtual ~IImageSaver();

	virtual bool saveToFile(const Properties &properties, const char *filename) = 0;
	virtual bool saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle) = 0;

	static unsigned int imageFormatToBpp(const Format format);
	static unsigned int dataSize(const Properties &properties);

	/// Saves the image directly to a file, choosing the proper saver according to the extension and using format-specific defaults
	/*! \return `true` on success, `false` otherwise (including when the extension is not recognized) */
	static bool save(const Properties &properties, const char *filename);

  protected:
	void flipPixels(const Properties &properties, unsigned char *dest);

  private:
	/// Returns the proper image saver according to the file extension, using format-specific defaults
	static nctl::UniquePtr<IImageSaver> createSaver(const char *filename);
};

}

#endif
