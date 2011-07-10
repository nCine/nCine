#ifndef CLASS_NCSPRITE
#define CLASS_NCSPRITE

#include "ncSceneNode.h"
#include "ncTexture.h"
#include "ncRect.h"

/// A scene node representing a basic sprite
class ncSprite : public ncSceneNode
{
private:
	ncTexture *m_pTexture;
	/// Used as source rectangle by the sprite batch class
//	ncRect m_texRect;
	int m_iWidth;
	int m_iHeight;

public:
	ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY);
	ncSprite(ncTexture *pTexture, int iX, int iY);
	virtual ~ncSprite() { }

	/// Return sprite width
	inline int Width() const { return m_iWidth; }
	/// Return sprite height
	inline int Height() const { return m_iHeight; }
	/// Return sprite size
	inline ncPoint Size() const { return ncPoint(m_iWidth, m_iHeight); }

	/// Scale the sprite size
	inline void setScale(float fFactor)
	{
		m_iWidth = m_pTexture->Width() * fFactor;
		m_iHeight = m_pTexture->Height() * fFactor;
	}

	virtual void Update(unsigned long int ulInterval);

	inline static eNodeType sType() { return SPRITE_TYPE; }

	friend class ncRenderGraph;
};

#endif
