#ifndef CLASS_NCANIMATEDSPRITE
#define CLASS_NCANIMATEDSPRITE

#include "ncSprite.h"
#include "ncRectAnimation.h"

/// A class for an animated sprite
class ncAnimatedSprite : public ncSprite
{
  public:
	ncAnimatedSprite(ncSceneNode* pParent, ncTexture *pTexture);
	ncAnimatedSprite(ncTexture *pTexture);
	ncAnimatedSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY);
	ncAnimatedSprite(ncTexture *pTexture, int iX, int iY);
	virtual ~ncAnimatedSprite();

	/// Returns true if the current animation is paused
	inline bool IsPaused() const { return m_vAnims[m_iCurrentAnim]->IsPaused(); }
	/// Sets the pause state for the animation
	inline void SetPaused(bool bPaused) { m_vAnims[m_iCurrentAnim]->SetPaused(bPaused); }

	virtual void Update(float fInterval);

	// Adds a new animation
	void AddAnimation(ncRectAnimation* pAnim);
	// Sets the current animation and its frame number
	void SetAnimation(int iAnimNum);
	/// Sets the current animation to a specified frame number
	inline void SetFrame(int iFrameNum) { m_vAnims[m_iCurrentAnim]->SetFrame(iFrameNum); }

	inline static eObjectType sType() { return ANIMATEDSPRITE_TYPE; }

  private:
	ncArray<ncRectAnimation *> m_vAnims;
	int m_iCurrentAnim;

	void Init();
};

#endif
