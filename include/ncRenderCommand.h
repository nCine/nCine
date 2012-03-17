#ifndef CLASS_NCRENDERCOMMAND
#define CLASS_NCRENDERCOMMAND

#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif !defined(NO_GLEW)
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#include "ncList.h"
#include "ncColor.h"

/// The class wrapping rendering material data
class ncRenderMaterial
{
private:
	ncColor m_color;
	GLuint m_uTextureGLId;

public:
	ncRenderMaterial(GLubyte ubColor[4], GLuint uTextureGLId)
		: m_uTextureGLId(uTextureGLId)
	{
		m_color.Setv(ubColor);
	}

	ncRenderMaterial(GLuint uTextureGLId) : m_uTextureGLId(uTextureGLId) { }
	ncRenderMaterial() : m_uTextureGLId(0) { }

	/// Sets the material color
	void SetColor(const ncColor &rColor)
	{
		m_color = rColor;
	}

	/// Sets the material color in unsigned bytes components
	void SetColor(GLubyte ubR, GLubyte ubG, GLubyte ubB, GLubyte ubA)
	{
		m_color.Set(ubR, ubG, ubB, ubA);
	}

	/// Sets the material color through an array
	void SetColor(GLubyte ubColor[4])
	{
		m_color.Setv(ubColor);
	}

	/// Sets the material color in float components
	void SetColor(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA)
	{
		m_color.SetF(fR, fG, fB, fA);
	}

	/// Sets the material color through an array
	void SetColor(GLfloat fColor[4])
	{
		m_color.SetFv(fColor);
	}

	/// Returns the material texture id
	inline GLuint TextureGLId() const { return m_uTextureGLId; }
	/// Sets the material texture id
	inline void SetTextureGLId(GLuint uTextureGLId) { m_uTextureGLId = uTextureGLId; }

	// Binds the material state
	void Bind() const;
};

/// The class wrapping geometric transformation
class ncRenderTransformation
{
private:
	float m_fX;
	float m_fY;
	float m_fScaleX;
	float m_fScaleY;
	float m_fRotation;

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
};

/// The class wrapping vertices data
class ncRenderGeometry
{
protected:
	GLenum m_eDrawType;
	GLint m_iFirstVertex;
	GLsizei m_iNumVertices;
	GLfloat *m_fVertices;
	GLfloat *m_fTexCoords;
	GLubyte *m_ubColors;

public:
	ncRenderGeometry(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords)
		: m_eDrawType(eDrawType),
		  m_iFirstVertex(iFirstVertex), m_iNumVertices(iNumVertices),
		  m_fVertices(fVertices), m_fTexCoords(fTexCoords), m_ubColors(NULL) { }
	ncRenderGeometry(GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords)
		: m_eDrawType(GL_TRIANGLES),
		  m_iFirstVertex(0), m_iNumVertices(iNumVertices),
		  m_fVertices(fVertices), m_fTexCoords(fTexCoords), m_ubColors(NULL) { }
	ncRenderGeometry()
		: m_eDrawType(GL_TRIANGLES),
		  m_iFirstVertex(0), m_iNumVertices(0),
		  m_fVertices(NULL), m_fTexCoords(NULL), m_ubColors(NULL) { }

	/// Sets the geometric data
	void SetData(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords, GLubyte *ubColors)
	{
		m_eDrawType = eDrawType;
		m_iFirstVertex = iFirstVertex;
		m_iNumVertices = iNumVertices;
		m_fVertices = fVertices;
		m_fTexCoords = fTexCoords;
		m_ubColors = ubColors;
	}

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
};

/// The class wrapping all the information needed for issuing a draw command
class ncRenderCommand
{
public:
	/// The enumeration of command types
	/*! Its sole purpose is to allow separated profiling counters in ncRenderQueue */
	enum eCommandType {
		GENERIC_TYPE = 0,
		PLOTTER_TYPE,
		SPRITE_TYPE,
		PARTICLE_TYPE,
		TEXT_TYPE,
		TYPE_COUNT
	};

private:
	unsigned long int m_uSortKey;
//	bool m_bDirtyKey;

	unsigned int m_iPriority;
	ncRenderMaterial m_material;
	ncRenderTransformation m_transformation;
	ncRenderGeometry m_geometry;

	/// Command type for profiling counter
	eCommandType m_eProfilingType;
public:
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
};

#endif
