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
	/// Logging related configuration
	struct Logging
	{
		/// The path for the log file
		nctl::String file{ 128 };
		/// The minimum logging level for messages printed on the console
		ILogger::LogLevel consoleLevel = ILogger::LogLevel::OFF;
		/// The minimum logging level for messages written in the log file
		ILogger::LogLevel fileLevel = ILogger::LogLevel::OFF;
		/// The interval in seconds for frame timer accumulation average and log
		float frameTimerInterval = 5.0f;
		/// Enables the use of colors for console log messages
		bool consoleColors = true;
	};

	/// Window related configuration
	struct Window
	{
		/// This special value can be used for window position coordinates when a specific position is not needed
		static const int IgnorePosition = 2147483647; // `INT_MAX`

		/// The window size or the resolution of the closest video mode if going fullscreen
		/*! \note If either `x` or `y` are zero or negative, the application will go in fullscreen at the current screen resolution. */
		Vector2i resolution{ 1280, 720 };
		/// The refresh rate of the screen mode used to go in fullscreen
		/*! \note If it is zero or negative, when requesting a fullscreen mode the current refresh rate will not be changed. */
		float refreshRate = 0.0f;
		/// The window position coordinates in the virtual screen made of all the connected monitors
		/*! \note It can also be used to go fullscreen on a monitor that is not the primary one of the system. */
		/*! \note The `IgnorePosition` value can be used in either or both coordinates when a specific position is not needed. */
		Vector2i position{ IgnorePosition, IgnorePosition };
		/// Enables fullscreen mode for the application window
		bool fullscreen = false;
		/// Allows the window to be resized by the user
		bool resizable = false;
		/// Enables automatic window size scaling according to the display factor
		bool scaling = true;

		/// The window title
		nctl::String title{ 128 };
		/// The window icon filename
		nctl::String iconFilename{ 128 };
	};

	/// Graphics related configuration
	struct Graphics
	{
		/// OpenGL context capabilities (determined at compile time)
		struct OpenGLCapabilities
		{
			/// Whether the OpenGL context uses a core profile
			bool coreProfile;
			/// Whether the OpenGL context is forward-compatible
			bool forwardCompatible;
			/// Major version of the OpenGL context
			unsigned int majorVersion;
			/// Minor version of the OpenGL context
			unsigned int minorVersion;

			/// Initializes compile-time OpenGL context capabilities
			OpenGLCapabilities();
		};

		/// OpenGL related configuration
		struct OpenGL
		{
			/// Enables the OpenGL debug context for additional validation and diagnostics
			bool debugContext = false;
			/// Enables buffer mapping for updating OpenGL buffers
			bool useBufferMapping = false;

			/** @name Resources */
			///@{

			/// The maximum size in bytes for each VBO collecting geometry data
			unsigned long vboSize = 64 * 1024;
			/// The maximum size in bytes for each IBO collecting index data
			unsigned long iboSize = 8 * 1024;
			/// The maximum size for the pool of VAOs
			unsigned int vaoPoolSize = 16;
			/// The initial size for the pool of render commands
			unsigned int renderCommandPoolSize = 32;

			///@}

			/** @name Shaders */
			///@{

			/// Defers shader program validation and introspection until first use
			/*! \note The value is only taken into account when the scenegraph is being used */
			bool deferShaderQueries = true;
			/// Fixed size of render commands to be collected for batching on Emscripten and ANGLE
			/*! \note Increasing this value too much might negatively affect batching shaders compilation time.
			A value of zero restores the default behavior of non fixed size for batches.
			\warning If this value is changed, the binary shader cache needs to be manually cleared. */
			unsigned int fixedBatchSize = 0;
			/// Enables the cache for loading and saving binary shader programs
			/*! \note Even if the flag is set to `true`, the functionality might still not be supported by the OpenGL context. */
			bool useBinaryShaderCache = false;
			/// The directory name (not the complete path) for the binary shaders cache
			nctl::String shaderCacheDirname{ 64 };
			/// Compiles batched shaders twice on devices with small UBO limits to determine maximum batch size
			/*! \note When enabled, compatibility with such devices will increase because shaders with standard batch size will not compile */
			bool compileBatchedShadersTwice = true;

			///@}
		};

		/// Enables vertical synchronization
		bool vsync = true;
		/// The maximum number of frames to render per second or 0 for no limit
		unsigned int frameLimit = 0;

		OpenGL opengl;
		const OpenGLCapabilities openglCaps;
	};

	/// Audio related configuration
	struct Audio
	{
		/// Enables the audio subsystem
		bool enabled = true;
		/// The output frequency of the audio system
		/*! \note Set this value to zero for the default output frequency of the device. */
		unsigned int frequency = 0;
		/// The number of mono audio sources
		/*! \note Set this value to zero to request the default number of mono audio sources. */
		unsigned int monoSources = 31;
		/// The number of stereo audio sources
		/*! \note Set this value to zero to request the default number of stereo audio sources. */
		unsigned int stereoSources = 1;
	};

	/// Job system related configuration
	struct JobSystem
	{
		/// Enables the job system
		bool enabled = true;
		/// The number of threads in the job system pool, or 0 for an automatic value
		unsigned int numThreads = 0;
	};

	/// Feature switches
	struct Features
	{
		/// Enables scenegraph-based rendering
		bool scenegraph = true;
		/// Enables the debug overlay
		bool debugOverlay = false;
	};

	Logging logging;
	Window window;
	Graphics graphics;
	Audio audio;
	JobSystem jobSystem;
	Features features;

	/// \returns The update time in seconds for the profile text nodes
	/*! \note It can only be changed at compile time */
	const float profileTextUpdateTime;

	/// Default constructor setting the defaults
	AppConfiguration();

	/// \returns The constant path for the application to load data from
	const nctl::String &dataPath() const;
	/// \returns The path for the application to load data from
	nctl::String &dataPath();

	/// \returns The number of arguments passed on the command line
	inline int argc() const { return argc_; }
	/// \returns The selected argument from the ones passed on the command line
	const char *argv(int index) const;

  private:
	/// A structure holding the variables that can be overridden by environment ones
	struct OldValues
	{
		Logging logging;
		Window window;
		Graphics graphics;
		Audio audio;
		JobSystem jobSystem;
		Features features;

		nctl::String dataPath;

		void assign(const AppConfiguration &appCfg);
	};

	int argc_;
	char **argv_;

	/// The old variables before a potential overriding by environment ones
	struct OldValues old_;

	/// Applies the configuration defaults
	void applyDefaults();
	/// Applies the platform overrides
	void applyOverrides();

	/// Logs the class fields
	void logFields() const;
	/// Reads the environment variables that can override the user ones
	void readEnvVariables();
	/// Logs the environment variables that have overridden the user ones
	void logEnvVariables() const;

	friend class PCApplication;
	friend class AndroidApplication;
};

}

#endif
