#ifndef CLASS_NCSPRITE
#define CLASS_NCSPRITE

#include "ncDrawableNode.h"
#include "ncTexture.h"
#include "ncRect.h"
#include "ncColor.h"
class ncSpriteBatchNode;

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

	float m_fVertices[8];
	float m_fTexCoords[8];
	void Init();
	void SetVertices();
	void SetTexCoords();

	virtual void UpdateRenderCommand();

	friend class ncSpriteBatchNode;
public:
	ncSprite(ncSceneNode* pParent, ncTexture *pTexture);
	ncSprite(ncTexture *pTexture);
	ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY);
	ncSprite(ncTexture *pTexture, int iX, int iY);
	virtual ~ncSprite() { }

	/// Returns sprite width
	inline int Width() const { return m_iWidth; }
	/// Returns sprite height
	inline int Height() const { return m_iHeight; }
	/// Returns sprite size
	inline ncPoint Size() const
	{
		return ncPoint(m_iWidth, m_iHeight);
	}
	/// Returns sprite rectangle
	inline ncRect Rect() const
	{
		float fHalfAbsW = m_iWidth * m_fAbsScaleFactor * 0.5f;
		float fHalfAbsH = m_iHeight * m_fAbsScaleFactor * 0.5f;
		return ncRect(x-fHalfAbsW, y-fHalfAbsH, fHalfAbsW*2.0f, fHalfAbsH*2.0f);
	}

	/// Sets sprite width
	inline void SetWidth(int iWidth) { m_iWidth = iWidth; }
	/// Sets sprite height
	inline void SetHeight(int iHeight) { m_iHeight = iHeight; }
	/// Sets sprite size
	inline void SetSize(const ncPoint &rSize)
	{
		m_iWidth = rSize.x;
		m_iHeight = rSize.y;
	}

	/// Returns sprite absolute width
	inline int AbsWidth() const { return m_iWidth * m_fAbsScaleFactor; }
	/// Returns sprite absolute height
	inline int AbsHeight() const { return m_iHeight * m_fAbsScaleFactor; }
	/// Returns sprite absolute size
	inline ncPoint AbsSize() const
	{
		return ncPoint(m_iWidth*m_fAbsScaleFactor, m_iHeight*m_fAbsScaleFactor);
	}

	/// Gets the texture object
	inline const ncTexture* Texture() const { return m_pTexture; }
	/// Sets the texture object
	inline void SetTexture(ncTexture *pTexture) { m_pTexture = pTexture; }

	/// Gets the texture source rectangle for blitting
	inline ncRect TexRect() const { return m_texRect; }
	/// Sets the texture source rectangle for blitting
	inline void SetTexRect(const ncRect& rect)
	{
		m_texRect = rect;
		m_iHeight = rect.h;
		m_iWidth = rect.w;
		SetTexCoords();
	}

	/// Flips the texture rect along the X coordinate
	inline void FlipX() {
		m_texRect.x += m_texRect.w;
		m_texRect.w *= -1;
		SetTexCoords();
	}

	/// Flips the texture rect along the Y coordinate
	inline void FlipY() {
		m_texRect.y += m_texRect.h;
		m_texRect.h *= -1;
		SetTexCoords();
	}

	inline static eObjectType sType() { return SPRITE_TYPE; }
};

#endif
