#ifndef CLASS_NCINE_RENDERRESOURCES
#define CLASS_NCINE_RENDERRESOURCES

#include "GLBufferObject.h"
#include "GLShaderProgram.h"

namespace ncine {

/// The class that creates and handles application common OpenGL rendering resources
class RenderResources
{
  public:
	/// A vertex format structure for vertices with positions only
	typedef struct
	{
		GLfloat position[2];
	} VertexFormatPos2;

	/// A vertex format structure for vertices with positions and texture coordinates
	typedef struct
	{
		GLfloat position[2];
		GLfloat texcoords[2];
	} VertexFormatPos2Tex2;

	/// Returns the OpenGL VBO of a quad made of two triangles, used by every sprite
	static inline const GLBufferObject *quadVbo() { return quadVbo_; }
	static inline const GLShaderProgram *spriteShaderProgram() { return spriteShaderProgram_; }
	static inline const GLShaderProgram *textnodeGrayShaderProgram() { return textnodeGrayShaderProgram_; }
	static inline const GLShaderProgram *textnodeColorShaderProgram() { return textnodeColorShaderProgram_; }
	static inline const GLShaderProgram *colorShaderProgram() { return colorShaderProgram_; }

  private:
	static GLBufferObject *quadVbo_;
	static GLShaderProgram *spriteShaderProgram_;
	static GLShaderProgram *textnodeGrayShaderProgram_;
	static GLShaderProgram *textnodeColorShaderProgram_;
	static GLShaderProgram *colorShaderProgram_;

	static void create();
	static void dispose();

	/// Static class, deleted constructor
	RenderResources() = delete;
	/// Static class, deleted copy constructor
	RenderResources(const RenderResources &other) = delete;
	/// Static class, deleted assignement operator
	RenderResources &operator=(const RenderResources &other) = delete;

	/// The `Application` class needs to create and dispose the resources
	friend class Application;
};

}

#endif
