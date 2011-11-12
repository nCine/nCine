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


	void SetColor(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA)
	{
		m_color.SetF(fR, fG, fB, fA);
	}

	void SetColor(GLfloat fColor[4])
	{
		m_color.SetFv(fColor);
	}

	inline GLuint TextureGLId() const { return m_uTextureGLId; }
	inline void SetTextureGLId(GLuint uTextureGLId) { m_uTextureGLId = uTextureGLId; }

	void Bind() const;
};

class ncRenderTransformation
{
private:
	float m_fX;
	float m_fY;
	float m_fScaleX;
	float m_fScaleY;

public:
	ncRenderTransformation() : m_fX(0.0f), m_fY(0.0f), m_fScaleX(1.0f), m_fScaleY(1.0f) { }

	ncRenderTransformation(float fX, float fY)
		: m_fX(fX), m_fY(fY), m_fScaleX(1.0f), m_fScaleY(1.0f) { }

	void SetPosition(float fX, float fY)
	{
		m_fX = fX;		m_fY = fY;
	}

	void SetScale(float fScaleX, float fScaleY)
	{
		m_fScaleX = fScaleX;	m_fScaleY = fScaleY;
	}

	void Apply() const;
};

class ncRenderGeometry
{
protected:
	GLenum m_eDrawType;
	GLint m_iFirstVertex;
	GLsizei m_iNumVertices;
	GLfloat *m_fVertices;
	GLfloat *m_fTexCoords;
	GLfloat *m_fColors;

public:
	ncRenderGeometry(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords)
		: m_eDrawType(eDrawType),
		  m_iFirstVertex(iFirstVertex), m_iNumVertices(iNumVertices),
		  m_fVertices(fVertices), m_fTexCoords(fTexCoords), m_fColors(NULL) { }
	ncRenderGeometry(GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords)
		: m_eDrawType(GL_TRIANGLES),
		  m_iFirstVertex(0), m_iNumVertices(iNumVertices),
		  m_fVertices(fVertices), m_fTexCoords(fTexCoords), m_fColors(NULL) { }
	ncRenderGeometry()
		: m_eDrawType(GL_TRIANGLES),
		  m_iFirstVertex(0), m_iNumVertices(0),
		  m_fVertices(NULL), m_fTexCoords(NULL), m_fColors(NULL) { }

	void SetData(GLenum eDrawType, GLint iFirstVertex, GLsizei iNumVertices, GLfloat *fVertices, GLfloat *fTexCoords, GLfloat *fColors)
	{
		m_eDrawType = eDrawType;
		m_iFirstVertex = iFirstVertex;
		m_iNumVertices = iNumVertices;
		m_fVertices = fVertices;
		m_fTexCoords = fTexCoords;
		m_fColors = fColors;
	}

	inline GLenum DrawType() const { return m_eDrawType; }
	inline GLint FirstVertex() const { return m_iFirstVertex; }
	inline GLsizei NumVertices() const { return m_iNumVertices; }
	inline GLfloat* VertexPointer() const { return m_fVertices; }
	inline GLfloat* TexCoordsPointer() const { return m_fTexCoords; }
	inline GLfloat* ColorPointer() const { return m_fColors; }

	void Draw() const;
};

/// The class wrapping all the information needed for issuing a draw command
class ncRenderCommand
{
private:
	unsigned long int m_uSortKey;
//	bool m_bDirtyKey;

	unsigned int m_iPriority;
	ncRenderMaterial m_material;
	ncRenderTransformation m_transformation;
	ncRenderGeometry m_geometry;

public:
	inline int Priority() const { return m_iPriority; }
	inline void SetPriority(int iPriority) { m_iPriority = iPriority; }
	inline const ncRenderMaterial& Material() const { return m_material; }
	inline ncRenderMaterial& Material() { return m_material; }
	inline const ncRenderTransformation& Transformation() const { return m_transformation; }
	inline ncRenderTransformation& Transformation() { return m_transformation; }
	inline const ncRenderGeometry& Geometry() const { return m_geometry; }
	inline ncRenderGeometry& Geometry() { return m_geometry; }

	inline unsigned long int SortKey() const { return m_uSortKey; }
	void CalculateSortKey();
	void Issue() const;
};

#endif
