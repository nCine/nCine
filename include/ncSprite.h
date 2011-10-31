#ifndef CLASS_NCSPRITE
#define CLASS_NCSPRITE

#include "ncDrawableNode.h"
#include "ncTexture.h"
#include "ncRect.h"
#include "ncColor.h"

class ncRenderQueue;

/// A scene node representing a basic sprite
class ncSprite : public ncDrawableNode
{
private:
	/// The sprite texture
	ncTexture *m_pTexture;
	/// Used as source rectangle by the sprite batch class
	ncRect m_texRect;
	/// Sprite width in pixel
	int m_iWidth;
	/// Sprite height in pixel
	int m_iHeight;
	/// Scale factor for sprite size
	float m_fScaleFactor;
	/// Sprite color (transparency, translucency, etc..)
	ncColor m_color;

	float m_fVertices[12];
	float m_fTexCoords[12];
	void Init();
	void SetVertices();
	void SetTexCoords();

	virtual void UpdateRenderCommand();
protected:

public:
	ncSprite(ncSceneNode* pParent, ncTexture *pTexture);
	ncSprite(ncTexture *pTexture);
	ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY);
	ncSprite(ncTexture *pTexture, int iX, int iY);
	virtual ~ncSprite() { }

	/// Return sprite width
	inline int Width() const { return m_iWidth * m_fScaleFactor; }
	/// Return sprite height
	inline int Height() const { return m_iHeight * m_fScaleFactor; }
	/// Return sprite size
	inline ncPoint Size() const
	{
		return ncPoint(m_iWidth*m_fScaleFactor, m_iHeight*m_fScaleFactor);
	}

	/// Get the sprite scale factor
	inline float Scale() const { return m_fScaleFactor; }
	/// Scale the sprite size
	inline void SetScale(float fScaleFactor) { m_fScaleFactor = fScaleFactor; }

	/// Get the texture object
	inline const ncTexture* Texture() { return m_pTexture; }
	/// Set the texture object
	inline void SetTexture(ncTexture *pTexture) { m_pTexture = pTexture; }

	/// Get the texture source rectangle for blitting
	inline ncRect TexRect() const { return m_texRect; }
	/// Set the texture source rectangle for blitting
	inline void SetTexRect(const ncRect& rect)
	{
		m_texRect = rect;
		m_iHeight = rect.h;
		m_iWidth = rect.w;
		SetTexCoords();
	}

	/// Get the sprite color
	inline const ncColor Color() { return m_color; }
	/// Set the sprite color through a ncColor class
	inline void SetColor(ncColor color) { m_color = color; }
	/// Set the sprite color through unsigned char components
	inline void SetColor(unsigned char ucR, unsigned char ucG, unsigned char ucB, unsigned char ucA) { m_color.Set(ucR, ucG, ucB, ucA); }
	/// Set the sprite color through float components
	inline void SetColor(float fR, float fG, float fB, float fA) { m_color.SetF(fR, fG, fB, fA); }


	inline static eObjectType sType() { return SPRITE_TYPE; }
	static ncSprite* FromId(unsigned int uId);
};

#endif
