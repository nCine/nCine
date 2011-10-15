#ifndef CLASS_NCSPRITE
#define CLASS_NCSPRITE

#include "ncDrawableNode.h"
#include "ncTexture.h"
#include "ncRect.h"

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

	float m_fVertices[12];
	float m_fTexCoords[12];
	void Init();
	void SetVertices();
	void SetTexCoords();
public:
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

	inline static eObjectType sType() { return SPRITE_TYPE; }
	static ncSprite* FromId(unsigned int uId);

	virtual void UpdateRenderCommand();

	friend class ncRenderGraph;
	friend class ncSpriteSheet;
};

#endif
