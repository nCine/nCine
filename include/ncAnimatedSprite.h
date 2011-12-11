#ifndef CLASS_NCANIMATEDSPRITE
#define CLASS_NCANIMATEDSPRITE

#include "ncSprite.h"
#include "ncRectAnimation.h"

/// A class for an animated sprite
class ncAnimatedSprite : public ncSprite
{
private:
	ncArray<ncRectAnimation *> m_vAnims;
	int m_iCurrentAnim;

	void Init();

public:
	ncAnimatedSprite(ncSceneNode* pParent, ncTexture *pTexture) : ncSprite(pParent, pTexture), m_vAnims(4), m_iCurrentAnim(-1) { }
	ncAnimatedSprite(ncTexture *pTexture) : ncSprite(pTexture), m_vAnims(4), m_iCurrentAnim(-1) { }
	ncAnimatedSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY) : ncSprite(pParent, pTexture, iX, iY), m_vAnims(4), m_iCurrentAnim(-1) { }
	ncAnimatedSprite(ncTexture *pTexture, int iX, int iY) : ncSprite(pTexture, iX, iY), m_vAnims(4), m_iCurrentAnim(-1) { }
	virtual ~ncAnimatedSprite();

	/// Returns true if the current animation is paused
	inline bool IsPaused() const { return m_vAnims[m_iCurrentAnim]->IsPaused(); }
	/// Sets the pause state for the animation
	inline void SetPaused(bool bPaused) { m_vAnims[m_iCurrentAnim]->SetPaused(bPaused); }

	virtual void Update(unsigned long int ulInterval);

	// Adds a new animation
	void AddAnimation(ncRectAnimation* pAnim);
	// Sets the current animation and its frame number
	void SetAnimation(int iAnimNum);
	/// Sets the current animation to a specified frame number
	inline void SetFrame(int iFrameNum) { m_vAnims[m_iCurrentAnim]->SetFrame(iFrameNum); }

	inline static eObjectType sType() { return ANIMATEDSPRITE_TYPE; }
	static ncAnimatedSprite* FromId(unsigned int uId);
};

#endif
