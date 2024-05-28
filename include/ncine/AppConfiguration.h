#ifndef CLASS_NCINE_APPCONFIGURATION
#define CLASS_NCINE_APPCONFIGURATION

#include <nctl/String.h>
#include "ILogger.h"
#include "Vector2.h"

namespace ncine {

/// The class storing initialization settings for an nCine application
class DLL_PUBLIC AppConfiguration
{
  public:
	static const int WindowPositionIgnore = 2147483647; // `INT_MAX`

	/// Default constructor setting the defaults
	AppConfiguration();

	// User configurable compile-time variables

	/// The path for the application to load data from
	nctl::String logFile;
	/// The logging level for messages printed on the console
	ILogger::LogLevel consoleLogLevel;
	/// The logging level for messages written in the log file
	ILogger::LogLevel fileLogLevel;
	/// The interval for frame timer accumulation average and log
	float frameTimerLogInterval;

	/// The window size or the resolution of the closest video mode if going full screen
	/*! \note If either `x` or `y` are zero or negative, the application will go in full screen at the current screen resolution. */
	Vector2i resolution;
	/// The refresh rate of the screen mode used to go in full screen
	/*! \note If it is zero or negative, when requesting a full screen mode the current refresh rate will not be changed. */
	float refreshRate;
	/// The window position coordinates in the virtual screen made of all the connected monitors
	/*! \note It can also be used to go full screen on a monitor that is not the primary one of the system. */
	/*! \note The `WindowPositionIgnore` value can be used in either or both dimensions when a specific position is not needed. */
	Vector2i windowPosition;
	/// The flag is `true` if the application is going to be in full screen mode
	bool fullScreen;
	/// The flag is `true` if the window is going to be resizable
	bool resizable;
	/// The flag is `true` if the window size is automatically scaled according to the display factor
	bool windowScaling;
	/// The maximum number of frames to render per second or 0 for no limit
	unsigned int frameLimit;

	/// The window title
	nctl::String windowTitle;
	/// The window icon filename
	nctl::String windowIconFilename;

	/// The flag is `true` if mapping is used to update OpenGL buffers
	bool useBufferMapping;
	/// The flag is `true` when error checking and introspection of shader programs are deferred to first use
	/*! \note The value is only taken into account when the scenegraph is being used */
	bool deferShaderQueries;
	/// Fixed size of render commands to be collected for batching on Emscripten and ANGLE
	/*! \note Increasing this value too much might negatively affect batching shaders compilation time.
	A value of zero restores the default behavior of non fixed size for batches.
	\warning If this value is changed, the binary shader cache needs to be manually cleared. */
	unsigned int fixedBatchSize;
	/// The flag is `true` if the shader cache is enabled to load and save binary shader programs
	/*! \note Even if the flag is `true` the functionality might still not be supported by the OpenGL context */
	bool useBinaryShaderCache;
	/// The directory name (not the complete path) for the binary shaders cache
	nctl::String shaderCacheDirname;
	/// The flag is `true` if, on devices with UBOs smaller than 64 KB, batched shaders will be compiled twice to identify their maximum batch size
	/*! \note When enabled, compatibility with such devices will increase because shaders with standard batch size will not compile */
	bool compileBatchedShadersTwice;

	/// The maximum size in bytes for each VBO collecting geometry data
	unsigned long vboSize;
	/// The maximum size in bytes for each IBO collecting index data
	unsigned long iboSize;
	/// The maximum size for the pool of VAOs
	unsigned int vaoPoolSize;
	/// The initial size for the pool of render commands
	unsigned int renderCommandPoolSize;

	/// The output frequency of the audio system
	/*! \note Set this value to zero for the default output frequency of the device. */
	unsigned int outputAudioFrequency;
	/// The number of mono audio sources
	/*! \note Set this value to zero to request the default number of mono audio sources. */
	unsigned int monoAudioSources;
	/// The number of stereo audio sources
	/*! \note Set this value to zero to request the default number of stereo audio sources. */
	unsigned int stereoAudioSources;

	/// The flag is `true` if the debug overlay is enabled
	bool withDebugOverlay;
	/// The flag is `true` if the audio subsystem is enabled
	bool withAudio;
	/// The flag is `true` if the threading subsystem is enabled
	bool withThreads;
	/// The flag is `true` if the scenegraph based rendering is enabled
	bool withScenegraph;
	/// The flag is `true` if the vertical synchronization is enabled
	bool withVSync;
	/// The flag is `true` if the OpenGL debug context is enabled
	bool withGlDebugContext;
	/// The flag is `true` if console log messages should use colors
	bool withConsoleColors;

	/// \returns The path for the application to load data from
	const nctl::String &dataPath() const;
	/// \returns The path for the application to load data from
	nctl::String &dataPath();

	/// \returns True if the OpenGL profile is going to be core
	inline bool glCoreProfile() const { return glCoreProfile_; }
	/// \returns True if the OpenGL context is going to be forward compatible
	inline bool glForwardCompatible() const { return glForwardCompatible_; }
	/// \returns The major version number of the OpenGL context
	inline unsigned int glMajorVersion() const { return glMajorVersion_; }
	/// \returns The minor version number of the OpenGL context
	inline unsigned int glMinorVersion() const { return glMinorVersion_; }
	/// \returns The update time in seconds for the profile text nodes
	inline float profileTextUpdateTime() const { return profileTextUpdateTime_; }

	/// \returns The number of arguments passed on the command line
	inline int argc() const { return argc_; }
	/// \returns The selected argument from the ones passed on the command line
	const char *argv(int index) const;

  private:
	// Pre-configured compile-time variables
	const bool glCoreProfile_;
	const bool glForwardCompatible_;
	const unsigned int glMajorVersion_;
	const unsigned int glMinorVersion_;
	const float profileTextUpdateTime_;

	int argc_;
	char **argv_;

	friend class PCApplication;
};

}

#endif
