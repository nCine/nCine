#ifndef CLASS_NCINE_CRASHPAD
#define CLASS_NCINE_CRASHPAD

#include "common_defines.h"
#include <nctl/Array.h>
#include <nctl/String.h>

namespace crashpad {

class CrashpadClient;

}

namespace ncine {

/// A class the integration of Google Crashpad
class DLL_PUBLIC Crashpad
{
  public:
	/// Called automatically after the `onPreInit()` callback, but the user can call it beforehand
	static bool initialize();
	/// It can be called to simulate a crash with a report dump
	static void simulateCrash();

	/// Returns true if the Crashpad handler has been correctly initialized
	inline static bool isInitialized() { return isInitialized_; }

	/// Returns true if the Crashpad handler is going to be initialized
	inline static bool &shouldInitialize() { return shouldInitialize_; }
	/// Sets the flag to automatically initialize the Crashpad handler (true by default)
	static void setShouldInitialize(bool shouldInitialize);

	/// Returns the path to the directory containing the Crashpad handler (not used by default)
	inline static const nctl::String &handlerPath() { return handlerPath_; }
	/// Sets the path to the directory containing the Crashpad handler
	static void setHandlerPath(const char *handlerPath);

	/// Returns the Crashpad executable handler name (`crashpad_handler` by default)
	inline static const nctl::String &handlerName() { return handlerName_; }
	/// Sets the Crashpad executable handler name
	static void setHandlerName(const char *handlerName);

	/// Returns the absolute path to crash dumps directory (not used by default)
	inline static const nctl::String &dataPath() { return dataPath_; }
	/// Sets the absolute path to the crash dumps directory
	static void setDataPath(const char *dataPath);

	/// Returns the name of the directory containing the crash dumps (`crashpad` by default)
	inline static const nctl::String &dataDirectoryName() { return dataDirectoryName_; }
	/// Sets the name of the directory containing the crash dumps
	static void setDataDirectoryName(const char *dataDirectoryName);

	/// Returns the URL of the upload server
	inline static const nctl::String &severUrl() { return serverUrl_; }
	/// Sets the URL of the upload server
	static void setServerUrl(const char *serverUrl);

	/// Adds a new annotation to pass to the Crashpad handler
	static bool addAnnotation(const char *key, const char *value);

	/// Adds a new attachment file to pass to the Crashpad handler
	static bool addAttachment(const char *filename);

  private:
	static bool isInitialized_;

	static bool shouldInitialize_;
	static nctl::String handlerPath_;
	static nctl::String handlerName_;
	static nctl::String dataPath_;
	static nctl::String dataDirectoryName_;
	static nctl::String serverUrl_;
	static nctl::Array<nctl::String> annotations_;
	static nctl::Array<nctl::String> attachments_;

	static nctl::UniquePtr<crashpad::CrashpadClient> client_;
};

}

#endif
