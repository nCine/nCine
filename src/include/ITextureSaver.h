#ifndef CLASS_NCINE_ITEXTURESAVER
#define CLASS_NCINE_ITEXTURESAVER

#include <nctl/UniquePtr.h>

namespace ncine {

class IFile;

/// Texture saver interface class
class ITextureSaver
{
  public:
	enum class Format
	{
		RGB8,
		RGBA8,
		RGB_FLOAT
	};

	struct Properties
	{
		Properties()
		    : width(0), height(0), format(Format::RGB8), pixels(nullptr) {}

		int width;
		int height;
		Format format;
		void *pixels;
	};

	virtual ~ITextureSaver() {}

	virtual bool saveToFile(const Properties &properties, const char *filename) = 0;
	virtual bool saveToFile(const Properties &properties, nctl::UniquePtr<IFile> fileHandle) = 0;
};

}

#endif
