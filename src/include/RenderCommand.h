#ifndef CLASS_NCINE_RENDERCOMMAND
#define CLASS_NCINE_RENDERCOMMAND

#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif defined(WITH_GLEW)
	#include <GL/glew.h>
#elif defined(__APPLE__)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#include <cstdio> // for NULL

#include "List.h"
#include "Color.h"

namespace ncine {

/// The class wrapping rendering material data
class RenderMaterial
{
  public:
	RenderMaterial(GLubyte color[4], GLuint textureGLId)
		: textureGLId_(textureGLId), alwaysTransparent_(false)
	{
		color_.setVec(color);
	}

	RenderMaterial(GLuint textureGLId) : textureGLId_(textureGLId), alwaysTransparent_(false) { }
	RenderMaterial() : textureGLId_(0), alwaysTransparent_(false) { }

	/// Sets the material color
	inline void setColor(const Color &color) { color_ = color; }
	/// Sets the material color in unsigned bytes components
	inline void setColor(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
	{
		color_.set(red, green, blue, alpha);
	}
	/// Sets the material color through an array
	inline void setColor(GLubyte color[4])
	{
		color_.setVec(color);
	}

	/// Sets the material color in float components
	inline void setColorF(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		color_.setF(red, green, blue, alpha);
	}
	/// Sets the material color through an array
	inline void setColorF(GLfloat color[4])
	{
		color_.setFVec(color);
	}

	/// Returns the material texture id
	inline GLuint textureGLId() const { return textureGLId_; }
	/// Sets the material texture id
	inline void setTextureGLId(GLuint textureGLId) { textureGLId_ = textureGLId; }

	/// Returns true if the render commands has to go into the transparent queue
	inline bool isTransparent() const { return (alwaysTransparent_ || color_.a() < 255); }
	/// Sets the always transparent flag
	inline void setAlwaysTransparent(bool alwaysTransparent) { alwaysTransparent_ = alwaysTransparent; }

	// Binds the material state
	void bind() const;

  private:
	Color color_;
	GLuint textureGLId_;
	/// Flag to skip opaque check based on material color
	/** This is useful when using color arrays, like for sprite batches */
	bool alwaysTransparent_;
};

/// The class wrapping geometric transformation
class RenderTransformation
{
  public:
	RenderTransformation() : x_(0.0f), y_(0.0f), xScale_(1.0f), yScale_(1.0f), rotation_(0.0f) { }
	RenderTransformation(float x, float y)
		: x_(x), y_(y), xScale_(1.0f), yScale_(1.0f), rotation_(0.0f) { }

	/// Sets the position
	void setPosition(float x, float y)
	{
		x_ = x;		y_ = y;
	}

	/// Sets the scale factors
	void setScale(float xScale, float yScale)
	{
		xScale_ = xScale;	yScale_ = yScale;
	}

	/// Sets the rotation in radians
	void setRotation(float rotation) { rotation_ = rotation; }

	// Applies the transformation
	void apply() const;

  private:
	float x_;
	float y_;
	float xScale_;
	float yScale_;
	float rotation_;
};

/// The class wrapping vertices data
class RenderGeometry
{
  public:
	RenderGeometry(GLenum drawType, GLint firstVertex, GLsizei numVertices, GLfloat *vertices, GLfloat *texCoords);
	RenderGeometry(GLsizei numVertices, GLfloat *vertices, GLfloat *texCoords);
	RenderGeometry();

	// Sets the geometric data
	void setData(GLenum drawType, GLint firstVertex, GLsizei numVertices, GLfloat *vertices, GLfloat *texCoords, GLubyte *colors);

	/// Returns the drawing type (GL_TRIANGLE, GL_LINES, ...)
	inline GLenum drawType() const { return drawType_; }
	/// Returns the first vertex to draw from the arrays
	inline GLint firstVertex() const { return firstVertex_; }
	/// Returns the number of vertices
	inline GLsizei numVertices() const { return numVertices_; }
	/// Returns the pointer to vertices data
	inline GLfloat* vertexPointer() const { return vertices_; }
	/// Returns the pointer to texture coordinates data
	inline GLfloat* texCoordsPointer() const { return texCoords_; }
	/// Return the pointer to colors data
	inline GLubyte* colorPointer() const { return colors_; }

	// Draws the geometry
	void draw() const;

  protected:
	GLenum drawType_;
	GLint firstVertex_;
	GLsizei numVertices_;
	GLfloat *vertices_;
	GLfloat *texCoords_;
	GLubyte *colors_;
};

/// Sets the geometric data
inline void RenderGeometry::setData(GLenum drawType, GLint firstVertex, GLsizei numVertices, GLfloat *vertices, GLfloat *texCoords, GLubyte *colors)
{
	drawType_ = drawType;
	firstVertex_ = firstVertex;
	numVertices_ = numVertices;
	vertices_ = vertices;
	texCoords_ = texCoords;
	colors_ = colors;
}

/// The class wrapping all the information needed for issuing a draw command
class RenderCommand
{
  public:
	/// The enumeration of command types
	/*! Its sole purpose is to allow separated profiling counters in the RenderQueue class */
	enum CommandType
	{
		GENERIC_TYPE = 0,
		PLOTTER_TYPE,
		SPRITE_TYPE,
		PARTICLE_TYPE,
		TEXT_TYPE,
		TYPE_COUNT
	};

	RenderCommand() : sortKey_(0), priority_(0), profilingType_(GENERIC_TYPE) { }

	/// Returns the rendering priority
	inline int priority() const { return priority_; }
	/// Sets the rendering priority
	inline void setPriority(int priority) { priority_ = priority; }
	/// Returns the rendering material
	inline const RenderMaterial& material() const { return material_; }
	/// Sets the rendering material
	inline RenderMaterial& material() { return material_; }
	/// Returns the geometric transformation
	inline const RenderTransformation& transformation() const { return transformation_; }
	/// Sets the geometric transformation
	inline RenderTransformation& transformation() { return transformation_; }
	/// Returns the vertices data
	inline const RenderGeometry& geometry() const { return geometry_; }
	/// Sets the vertices data
	inline RenderGeometry& geometry() { return geometry_; }

	/// Returns the queue sort key
	inline unsigned long int sortKey() const { return sortKey_; }
	// Calculates a sort key for the queue
	void calculateSortKey();
	// Issues the render command
	void issue() const;

	/// Gets the command type (profiling purpose)
	inline CommandType type() const { return profilingType_; }
	/// Sets the command type (profiling purpose)
	inline void setType(CommandType eType) { profilingType_ = eType; }

  private:
	unsigned long int sortKey_;

	unsigned int priority_;
	RenderMaterial material_;
	RenderTransformation transformation_;
	RenderGeometry geometry_;

	/// Command type for profiling counter
	CommandType profilingType_;
};

}

#endif
