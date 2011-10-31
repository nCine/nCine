#ifndef CLASS_NCPARTICLE
#define CLASS_NCPARTICLE

#include "ncSprite.h"
#include "ncVector2f.h"
#include "ncColor.h"
class ncPoint;
class ncTexture;

class ncParticle : public ncSprite
{
public:
	unsigned long int m_ulLife;
	unsigned long int m_ulStartLife; // for affectors
	ncVector2f m_Velocity;

	ncParticle(ncSceneNode* pParent, ncTexture *pTexture) : ncSprite(pParent, pTexture), m_ulLife(0) { }
	void Init(unsigned long int ulLife, ncPoint pos, ncVector2f vel);

	virtual void Update(unsigned long int ulInterval);
};

#endif
