#ifndef CLASS_NCINE_SPRITEBATCHNODE
#define CLASS_NCINE_SPRITEBATCHNODE

#include "DrawableNode.h"
#include "RenderCommand.h"
#include "Texture.h"
#include "Array.h"

namespace ncine {

class Sprite;

/// A scene node reuniting more sprites with a common texture under a unique draw call
class DLL_PUBLIC SpriteBatchNode : public DrawableNode
{
  public:
	SpriteBatchNode(SceneNode* parent, Texture *texture);

	inline Texture* texture() { return texture_; }
	inline void setTexture(Texture* texture) { texture_ = texture; }

	virtual void visit(RenderQueue& renderQueue);

	inline static ObjectType sType() { return SPRITEBATCH_TYPE; }

  private:
	/// The spritesheet texture shared by every sprite in the batch
	Texture *texture_;
	/// The array of vertices for every sprite in the batch
	Array<float> vertices_;
	/// The array of texture coordinates for every sprite in the batch
	Array<float> texCoords_;
	/// The array of colors for every sprite in the batch
	Array<unsigned char> colors_;

	/// Fill the batch draw command with data from a sprite
	void processSprite(Sprite& sprite);

	virtual void updateRenderCommand();
};

}

#endif
