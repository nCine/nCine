#ifndef CLASS_NCRENDERCOMMAND
#define CLASS_NCRENDERCOMMAND

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

#include "ncList.h"
#include "ncColor.h"

/// The class wrapping rendering material data
class ncRenderMaterial
{
  public:
	ncRenderMaterial(GLubyte ubColor[4], GLuint uTextureGLId)
		: m_uTextureGLId(uTextureGLId), m_bAlwaysTransparent(false)
	{
		m_color.Setv(ubColor);
	}

	ncRenderMaterial(GLuint uTextureGLId) : m_uTextureGLId(uTextureGLId), m_bAlwaysTransparent(false) { }
	ncRenderMaterial() : m_uTextureGLId(0), m_bAlwaysTransparent(false) { }

	/// Sets the material color
	inline void SetColor(const ncColor &rColor)	{ m_color = rColor;	}
	/// Sets the material color in unsigned bytes components
	inline void SetColor(GLubyte ubR, GLubyte ubG, GLubyte ubB, GLubyte ubA)
	{
		m_color.Set(ubR, ubG, ubB, ubA);
	}
	/// Sets the material color through an array
	inline void SetColor(GLubyte ubColor[4])
	{
		m_color.Setv(ubColor);
	}

	/// Sets the material color in float components
	inline void SetColorF(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA)
	{
		m_color.SetF(fR, fG, fB, fA);
	}
	/// Sets the material color through an array
	inline void SetColorF(GLfloat fColor[4])
	{
		m_color.SetFv(fColor);
	}

	/// Returns the material texture id
	inline GLuint TextureGLId() const { return m_uTextureGLId; }
	/// Sets the material texture id
	inline void SetTextureGLId(GLuint uTextureGLId) { m_uTextureGLId = uTextureGLId; }

	/// Returns true if the render commands has to go into the transparent queue
	inline bool isTransparent() const { return (m_bAlwaysTransparent || m_color.A() < 255); }
	/// Sets the always transparent flag
	inline void SetAlwaysTransparent(bool bAlwaysTransparent) { m_bAlwaysTransparent = bAlwaysTransparent; }

	// Binds the material state
	void Bind() const;

  private:
	ncColor m_color;
	GLuint m_uTextureGLId;
	/// Flag to skip opaque check based on material color
	/** This is useful when using color arrays, like for sprite batches */
	bool m_bAlwaysTransparent;
};

/// The class wrapping geometric transformation
class ncRenderTransformation
{
  public:
	ncRenderTransformation() : m_fX(0.0f), m_fY(0.0f), m_fScaleX(1.0f), m_fScaleY(1.0f), m_fRotation(0.0f) { }

	ncRenderTransformation(float fX, float fY)
		: m_fX(fX), m_fY(fY), m_fScaleX(1.0f), m_fScaleY(1.0f), m_fRotation(0.0f) { }

	/// Sets the position
	void SetPosition(float fX, float fY)
	{
		m_fX = fX;		m_fY = fY;
	}

	/// Sets the scale factors
	void SetScale(float fScaleX, float fScaleY)
	{
		m_fScaleX = fScaleX;	m_fScaleY = fScaleY;
	}

	/// Sets the rotation in radians
	void SetRotation(float fRotation) { m_fRotation = fRotation; }

	// Applies the transformation
	void Apply() const;

  private:
	float m_fX;
	float m_fY;
	float m_fScaleX;
	float m_fScaleY;
	float m_fRotation;
};

/// The class wrapping vertices data
class ncRenderGeometry
{
  public:
	ncRenderGeometry(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords);
	ncRenderGeometry(GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords);
	ncRenderGeometry();

	// Sets the geometric data
	void SetData(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords, GLubyte *ubColors);

	/// Returns the drawing type (GL_TRIANGLE, GL_LINES, ...)
	inline GLenum DrawType() const { return m_eDrawType; }
	/// Returns the first vertex to draw from the arrays
	inline GLint FirstVertex() const { return m_iFirstVertex; }
	/// Returns the number of vertices
	inline GLsizei NumVertices() const { return m_iNumVertices; }
	/// Returns the pointer to vertices data
	inline GLfloat* VertexPointer() const { return m_fVertices; }
	/// Returns the pointer to texture coordinates data
	inline GLfloat* TexCoordsPointer() const { return m_fTexCoords; }
	/// Return the pointer to colors data
	inline GLubyte* ColorPointer() const { return m_ubColors; }

	// Draws the geometry
	void Draw() const;

  protected:
	GLenum m_eDrawType;
	GLint m_iFirstVertex;
	GLsizei m_iNumVertices;
	GLfloat *m_fVertices;
	GLfloat *m_fTexCoords;
	GLubyte *m_ubColors;
};

/// Sets the geometric data
inline void ncRenderGeometry::SetData(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords, GLubyte *ubColors)
{
	m_eDrawType = eDrawType;
	m_iFirstVertex = iFirstVertex;
	m_iNumVertices = iNumVertices;
	m_fVertices = fVertices;
	m_fTexCoords = fTexCoords;
	m_ubColors = ubColors;
}

/// The class wrapping all the information needed for issuing a draw command
class ncRenderCommand
{
  public:
	/// The enumeration of command types
	/*! Its sole purpose is to allow separated profiling counters in ncRenderQueue */
	enum eCommandType
	{
		GENERIC_TYPE = 0,
		PLOTTER_TYPE,
		SPRITE_TYPE,
		PARTICLE_TYPE,
		TEXT_TYPE,
		TYPE_COUNT
	};

	ncRenderCommand() : m_uSortKey(0), m_iPriority(0), m_eProfilingType(GENERIC_TYPE) { }

	/// Returns the rendering priority
	inline int Priority() const { return m_iPriority; }
	/// Sets the rendering priority
	inline void SetPriority(int iPriority) { m_iPriority = iPriority; }
	/// Returns the rendering material
	inline const ncRenderMaterial& Material() const { return m_material; }
	/// Sets the rendering material
	inline ncRenderMaterial& Material() { return m_material; }
	/// Returns the geometric transformation
	inline const ncRenderTransformation& Transformation() const { return m_transformation; }
	/// Sets the geometric transformation
	inline ncRenderTransformation& Transformation() { return m_transformation; }
	/// Returns the vertices data
	inline const ncRenderGeometry& Geometry() const { return m_geometry; }
	/// Sets the vertices data
	inline ncRenderGeometry& Geometry() { return m_geometry; }

	/// Returns the queue sort key
	inline unsigned long int SortKey() const { return m_uSortKey; }
	// Calculates a sort key for the queue
	void CalculateSortKey();
	// Issues the render command
	void Issue() const;

	/// Gets the command type (profiling purpose)
	inline eCommandType Type() const { return m_eProfilingType; }
	/// Sets the command type (profiling purpose)
	inline void SetType(eCommandType eType) { m_eProfilingType = eType; }

  private:
	unsigned long int m_uSortKey;
//	bool m_bDirtyKey;

	unsigned int m_iPriority;
	ncRenderMaterial m_material;
	ncRenderTransformation m_transformation;
	ncRenderGeometry m_geometry;

	/// Command type for profiling counter
	eCommandType m_eProfilingType;
};

#endif
