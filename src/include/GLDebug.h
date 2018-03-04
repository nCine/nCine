#ifndef CLASS_NCINE_GLDEBUG
#define CLASS_NCINE_GLDEBUG

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include "IGfxCapabilities.h"

namespace ncine {

/// A class to handle OpenGL debug functions
class GLDebug
{
  public:
	class ScopedGroup
	{
	  public:
		ScopedGroup(const char *message) { pushGroup(message); }
		~ScopedGroup() { popGroup(); }
	};

	static void init(const IGfxCapabilities &gfxCaps);
	static inline void reset() { debugGroupId_ = 0; }

	static void pushGroup(const char *message);
	static void popGroup();

	static void objectLabel(GLenum identifier, GLuint name, const char *label);
	static void objectLabel(GLenum identifier, GLuint name, GLsizei length, const char *label);
	static void getObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, char *label);

	static inline unsigned int maxLabelLength() { return maxLabelLength_; }

  private:
	static bool debugAvailable_;
	static GLuint debugGroupId_;
	static unsigned int maxLabelLength_;

	/// Enables OpenGL debug output and setup a callback function to log messages
	static void enableDebugOutput();
};

}

#endif
