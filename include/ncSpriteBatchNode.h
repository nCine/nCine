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
	ncSpriteBatchNode(ncSceneNode* parent, ncTexture *texture);
	virtual ~ncSpriteBatchNode() { }

	inline ncTexture* texture() { return texture_; }
	inline void setTexture(ncTexture* texture) { texture_ = texture; }

	virtual void visit(ncRenderQueue& renderQueue);

	inline static ObjectType sType() { return SPRITEBATCH_TYPE; }

  private:
	/// The spritesheet texture shared by every sprite in the batch
	ncTexture *texture_;
	/// The array of vertices for every sprite in the batch
	ncArray<float> vertices_;
	/// The array of texture coordinates for every sprite in the batch
	ncArray<float> texCoords_;
	/// The array of colors for every sprite in the batch
	ncArray<unsigned char> colors_;

	/// Fill the batch draw command with data from a sprite
	void processSprite(ncSprite& sprite);

	virtual void updateRenderCommand();
};

#endif
