#ifndef CLASS_NCSPRITEBATCHNODE
#define CLASS_NCSPRITEBATCHNODE

#include "ncDrawableNode.h"
#include "ncRenderCommand.h"
#include "ncTexture.h"
#include "ncArray.h"

class ncSprite;

/// A scene node reuniting more sprites with a common texture under a unique draw call
class ncSpriteBatchNode : public ncDrawableNode
{
 public:
	ncSpriteBatchNode(ncSceneNode* pParent, ncTexture *pTexture);
	virtual ~ncSpriteBatchNode() { }

	inline ncTexture* Texture() { return m_pTexture; }
	inline void SetTexture(ncTexture* pTexture) { m_pTexture = pTexture; }

	virtual void Visit(ncRenderQueue& rRenderQueue);

	inline static eObjectType sType() { return SPRITEBATCH_TYPE; }

 private:
	/// The spritesheet texture shared by every sprite in the batch
	ncTexture *m_pTexture;
	/// The array of vertices for every sprite in the batch
	ncArray<float> m_vVertices;
	/// The array of texture coordinates for every sprite in the batch
	ncArray<float> m_vTexCoords;
	/// The array of colors for every sprite in the batch
	ncArray<unsigned char> m_vColors;

	/// Fill the batch draw command with data from a sprite
	void ProcessSprite(ncSprite& rSprite);

	virtual void UpdateRenderCommand();
};

#endif
