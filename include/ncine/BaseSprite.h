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
	/// Default move constructor
	BaseSprite(BaseSprite &&) = default;
	/// Default move assignment operator
	BaseSprite &operator=(BaseSprite &&) = default;

	/// Sets sprite size
	void setSize(float width, float height);
	/// Sets sprite size with a `Vector2f`
	inline void setSize(const Vector2f &size) { setSize(size.x, size.y); }

	/// Gets the texture object
	inline const Texture *texture() const { return texture_; }
	/// Sets the texture object
	void setTexture(Texture *texture);
	/// Triggers a texture update without setting a new texture
	void resetTexture();

	/// Gets the texture source rectangle for blitting
	inline Recti texRect() const { return texRect_; }
	/// Sets the texture source rectangle for blitting
	void setTexRect(const Recti &rect);

	/// Returns `true` if the sprite texture is horizontally flipped
	inline bool isFlippedX() const { return flippedX_; }
	/// Flips the texture rect horizontally
	void setFlippedX(bool flippedX);
	/// Returns `true` if the sprite texture is vertically flipped
	inline bool isFlippedY() const { return flippedY_; }
	/// Flips the texture rect vertically
	void setFlippedY(bool flippedY);

  protected:
	/// The sprite texture
	Texture *texture_;
	/// The texture source rectangle
	Recti texRect_;

	/// A flag indicating if the sprite texture is horizontally flipped
	bool flippedX_;
	/// A flag indicating if the sprite texture is vertically flipped
	bool flippedY_;

	GLUniformBlockCache *instanceBlock_;

	/// Protected constructor accessible only by derived sprite classes
	BaseSprite(SceneNode *parent, Texture *texture, float xx, float yy);
	/// Protected constructor accessible only by derived sprite classes
	BaseSprite(SceneNode *parent, Texture *texture, const Vector2f &position);

	/// Protected copy constructor used to clone objects
	BaseSprite(const BaseSprite &other);

	/// Performs the required tasks upon a change to the shader
	void shaderHasChanged() override;

	/// Performs the required tasks upon a change to the texture
	virtual void textureHasChanged(Texture *newTexture) = 0;

	void updateRenderCommand() override;

  private:
	/// Deleted assignment operator
	BaseSprite &operator=(const BaseSprite &) = delete;
};

}

#endif
