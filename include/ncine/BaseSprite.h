#ifndef CLASS_NCINE_BASESPRITE
#define CLASS_NCINE_BASESPRITE

#include "DrawableNode.h"
#include "Rect.h"

namespace ncine {

class Texture;
class GLUniformBlockCache;

/// The base class for sprites
/*! \note Users cannot create instances of this class */
class DLL_PUBLIC BaseSprite : public DrawableNode
{
  public:
	/// Sets sprite size
	void setSize(float width, float height);
	/// Sets sprite size with a `Vector2f`
	inline void setSize(const Vector2f &size) { setSize(size.x, size.y); }

	/// Gets the texture object
	inline const Texture *texture() const { return texture_; }
	/// Sets the texture object
	void setTexture(Texture *texture);

	/// Gets the texture source rectangle for blitting
	inline Recti texRect() const { return texRect_; }
	/// Sets the texture source rectangle for blitting
	void setTexRect(const Recti &rect);

	/// Returns `true` if the sprite is horizontally flipped
	inline bool isFlippedX() const { return flippedX_; }
	/// Flips the texture rect horizontally
	void setFlippedX(bool flippedX);
	/// Returns `true` if the sprite is vertically flipped
	inline bool isFlippedY() const { return flippedY_; }
	/// Flips the texture rect vertically
	void setFlippedY(bool flippedY);

  protected:
	/// The sprite texture
	Texture *texture_;
	/// The texture source rectangle
	Recti texRect_;

	/// A flag indicating if the sprite is horizontally flipped
	bool flippedX_;
	/// A flag indicating if the sprite is vertically flipped
	bool flippedY_;

	GLUniformBlockCache *spriteBlock_;

	/// Protected construtor accessible only by derived sprite classes
	BaseSprite(SceneNode *parent, Texture *texture, float xx, float yy);
	/// Protected construtor accessible only by derived sprite classes
	BaseSprite(SceneNode *parent, Texture *texture, const Vector2f &position);

	void updateRenderCommand() override;

	/// Performs the required tasks upon a change in the texture
	virtual void textureHasChanged(Texture *newTexture) = 0;
};

}

#endif
