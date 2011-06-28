#ifndef CLASS_NCSPRITEBATCH
#define CLASS_NCSPRITEBATCH

#include "ncPoint.h"
#include "ncRect.h"
#include "ncTexture.h"

// 2 triangles forming a rectangle
// 3 vertices forming a triangle
// 2 components forming a vertex coordinate (both XY and UV)
#define QUAD_ELEMENTS 12 // TODO: very dirty

/// A class that sends a batch of sprites to the GPU for drawing
class ncSpriteBatch
{
private:
	unsigned int m_uBufferSize;
	/// Top of the buffer stack
	unsigned int m_uBufferTop;
	unsigned int m_uMaxBufferCounter;
	unsigned int m_uMaxRenderCallsCounter;

	GLfloat *m_fVertices;
	GLfloat *m_fTexCoords;
	ncTexture *m_pLastTexture;
	bool m_bDrawing;

	void FlushBuffer();
	void SetVertices(GLfloat *fVertices, ncRect rect);
	void SetTexCoords(GLfloat *fTexCoords, ncRect rect);
	void SetTexCoords(GLfloat *fTexCoords);
public:
	ncSpriteBatch(unsigned int uBufferSize = 10);
	~ncSpriteBatch();
	void Begin();
	void Draw(ncTexture *pTexture, ncRect dstRect, ncRect srcRect);
	void Draw(ncTexture *pTexture, ncRect dstRect);
	void Draw(ncTexture *pTexture, ncPoint pos);
	void End();

	/// Return the sprite buffer size
	inline unsigned int BufferSize() const { return m_uBufferSize; }
	/// Return the biggest number of sprites buffered during the object lifetime
	inline unsigned int MaxBuffered() const { return m_uMaxBufferCounter; }
	/// Return the biggest number of render calls during the object lifetime
	inline GLenum MaxRenderCalls() const { return m_uMaxRenderCallsCounter; }
	/// Return true if the class is busy drawing.
	inline bool isDrawing() const { return m_bDrawing; }
};

#endif
