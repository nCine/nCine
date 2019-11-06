// Based on https://github.com/msorvig/qt-webassembly-examples/emscripten_localfiles

#include <emscripten/bind.h>
#include "common_macros.h"
#include "EmscriptenLocalFile.h"

namespace ncine {

namespace {

	using FileDataCallbackType = void(void *context, char *data, size_t length, const char *name);
	using LoadingCallbackType = void(void *context);

	void readFileContent(emscripten::val event)
	{
		// Copy file content to WebAssembly memory and call the user file data handler
		emscripten::val fileReader = event["target"];

		// Set up source typed array
		emscripten::val result = fileReader["result"]; // ArrayBuffer
		emscripten::val Uint8Array = emscripten::val::global("Uint8Array");
		emscripten::val sourceTypedArray = Uint8Array.new_(result);

		// Allocate and set up destination typed array
		const size_t size = result["byteLength"].as<size_t>();
		char *buffer = new char[size];
		emscripten::val destinationTypedArray = Uint8Array.new_(emscripten::val::module_property("HEAPU8")["buffer"], size_t(buffer), size);
		destinationTypedArray.call<void>("set", sourceTypedArray);

		// Call user file data handler
		FileDataCallbackType *fileDataCallback = reinterpret_cast<FileDataCallbackType *>(fileReader["data-filesReadyCallback"].as<size_t>());
		void *context = reinterpret_cast<void *>(fileReader["data-callbackContext"].as<size_t>());
		fileDataCallback(context, buffer, size, fileReader["data-name"].as<std::string>().c_str());
	}

	void readFiles(emscripten::val event)
	{
		// Read all selected files using FileReader
		emscripten::val target = event["target"];
		emscripten::val files = target["files"];
		const int fileCount = files["length"].as<int>();

		if (fileCount > 0)
		{
			LoadingCallbackType *loadingCallback = reinterpret_cast<LoadingCallbackType *>(target["data-loadingCallback"].as<size_t>());
			void *context = reinterpret_cast<void *>(target["data-callbackContext"].as<size_t>());
			loadingCallback(context);
		}

		for (int i = 0; i < fileCount; i++)
		{
			emscripten::val file = files[i];
			emscripten::val fileReader = emscripten::val::global("FileReader").new_();
			fileReader.set("onload", emscripten::val::module_property("jsReadFileContent"));
			fileReader.set("data-filesReadyCallback", target["data-filesReadyCallback"]);
			fileReader.set("data-callbackContext", target["data-callbackContext"]);
			fileReader.set("data-name", file["name"]);
			fileReader.call<void>("readAsArrayBuffer", file);
		}
	}

	/// Loads a file by opening a native file dialog
	void loadFile(const char *accept, FileDataCallbackType *filesReadyCallback, LoadingCallbackType *loadingCallback, void *context)
	{
		// Create file input element which will display a native file dialog.
		emscripten::val document = emscripten::val::global("document");
		emscripten::val input = document.call<emscripten::val>("createElement", std::string("input"));
		input.set("type", "file");
		input.set("style", "display:none");
		input.set("accept", emscripten::val(accept));

		// Set JavaScript `onchange` callback which will be called on selected file(s),
		// and also forward the user C callback pointers so that the `onchange`
		// callback can call it. (The `onchange` callback is actually a C function
		// exposed to JavaScript with EMSCRIPTEN_BINDINGS).
		input.set("onchange", emscripten::val::module_property("jsReadFiles"));
		input.set("data-filesReadyCallback", emscripten::val(size_t(filesReadyCallback)));
		input.set("data-loadingCallback", emscripten::val(size_t(loadingCallback)));
		input.set("data-callbackContext", emscripten::val(size_t(context)));

		// Programatically activate input
		emscripten::val body = document["body"];
		body.call<void>("appendChild", input);
		input.call<void>("click");
		body.call<void>("removeChild", input);
	}

	/// Saves file by triggering a browser file download.
	void saveFile(const char *data, size_t length, const char *filenameHint)
	{
		// Create file data Blob
		emscripten::val Blob = emscripten::val::global("Blob");
		emscripten::val contentArray = emscripten::val::array();
		emscripten::val content = emscripten::val(emscripten::typed_memory_view(length, data));
		contentArray.call<void>("push", content);
		emscripten::val type = emscripten::val::object();
		type.set("type", "application/octet-stream");
		emscripten::val fileBlob = Blob.new_(contentArray, type);

		// Create Blob download link
		emscripten::val document = emscripten::val::global("document");
		emscripten::val link = document.call<emscripten::val>("createElement", std::string("a"));
		link.set("download", filenameHint);
		emscripten::val window = emscripten::val::global("window");
		emscripten::val URL = window["URL"];
		link.set("href", URL.call<emscripten::val>("createObjectURL", fileBlob));
		link.set("style", "display:none");

		// Programatically click link
		emscripten::val body = document["body"];
		body.call<void>("appendChild", link);
		link.call<void>("click");
		body.call<void>("removeChild", link);
	}

	EMSCRIPTEN_BINDINGS(localfileaccess)
	{
		function("jsReadFiles", &readFiles);
		function("jsReadFileContent", &readFileContent);
	};

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void EmscriptenLocalFile::load()
{
	loadFile("*", fileDataCallback, loadingCallback, this);
}

void EmscriptenLocalFile::load(const char *fileFilter)
{
	FATAL_ASSERT(fileFilter);
	loadFile(fileFilter, fileDataCallback, loadingCallback, this);
}

void EmscriptenLocalFile::save(const char *filename)
{
	FATAL_ASSERT(filename);
	ASSERT(fileSize_ > 0);

	LOGI_X("Saving file: \"%s\" (%u bytes)", filename, fileSize_);
	saveFile(fileBuffer_.get(), fileSize_, filename);
}

unsigned long int EmscriptenLocalFile::read(void *buffer, unsigned long int bytes) const
{
	FATAL_ASSERT(buffer);
	ASSERT(bytes > 0);

	memcpy(buffer, fileBuffer_.get(), bytes);
	return bytes;
}

unsigned long int EmscriptenLocalFile::write(void *buffer, unsigned long int bytes)
{
	FATAL_ASSERT(buffer);
	ASSERT(bytes > 0);

	fileBuffer_ = nctl::makeUnique<char[]>(bytes);
	memcpy(fileBuffer_.get(), buffer, bytes);
	fileSize_ = bytes;

	return bytes;
}

void EmscriptenLocalFile::setLoadedCallback(LoadedCallbackType *loadedCallback, void *userData)
{
	loadedCallback_ = loadedCallback;
	userData_ = userData;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void EmscriptenLocalFile::fileDataCallback(void *context, char *contentPointer, size_t contentSize, const char *filename)
{
	FATAL_ASSERT(context);
	FATAL_ASSERT(contentPointer);
	ASSERT(contentSize > 0);

	LOGI_X("Loading file: \"%s\" (%u bytes)", filename, contentSize);

	EmscriptenLocalFile *localFile = reinterpret_cast<EmscriptenLocalFile *>(context);
	localFile->fileBuffer_ = nctl::makeUnique<char[]>(contentSize);
	memcpy(localFile->fileBuffer_.get(), contentPointer, contentSize);
	delete[](contentPointer);

	localFile->fileSize_ = contentSize;
	localFile->filename_ = filename;
	localFile->loading_ = false;

	if (localFile->loadedCallback_)
		localFile->loadedCallback_(*localFile, localFile->userData_);
}

void EmscriptenLocalFile::loadingCallback(void *context)
{
	FATAL_ASSERT(context);

	EmscriptenLocalFile *localFile = reinterpret_cast<EmscriptenLocalFile *>(context);
	localFile->loading_ = true;
}

}
