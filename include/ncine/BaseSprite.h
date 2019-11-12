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
	/// Sets sprite width
	inline void setWidth(float width) { width_ = width; }
	/// Sets sprite height
	inline void setHeight(float height) { height_ = height; }
	/// Sets sprite size
	void setSize(float width, float height);
	/// Sets sprite size with a `Vector2f`
	inline void setSize(const Vector2f &size) { setSize(size.x, size.y); }

	/// Gets the texture object
	inline const Texture *texture() const { return texture_; }
	/// Sets the texture object
	inline void setTexture(Texture *texture) { texture_ = texture; }
	/// Sets a flag that makes a transparent texture to be considered opaque
	/*! \note This flag comes in handy when the sprite uses an opaque region of a transparent texture atlas. */
	inline void setOpaqueTexture(bool opaqueTexture) { opaqueTexture_ = opaqueTexture; }

	/// Gets the texture source rectangle for blitting
	inline Recti texRect() const { return texRect_; }
	/// Sets the texture source rectangle for blitting
	void setTexRect(const Recti &rect);

	/// Flips the texture rect along the X coordinate
	void flipX();
	/// Flips the texture rect along the Y coordinate
	void flipY();

  protected:
	/// Protected construtor accessible only by derived sprite classes
	BaseSprite(SceneNode *parent, Texture *texture, float xx, float yy);
	/// Protected construtor accessible only by derived sprite classes
	BaseSprite(SceneNode *parent, Texture *texture, const Vector2f &position);

	/// The sprite texture
	Texture *texture_;
	/// The texture source rectangle
	Recti texRect_;
	/// The opaque texture flag
	bool opaqueTexture_;

	GLUniformBlockCache *spriteBlock_;

	void updateRenderCommand() override;
};

}

#endif
