#ifndef CLASS_NCINE_SPRITE
#define CLASS_NCINE_SPRITE

#include "DrawableNode.h"
#include "Texture.h"
#include "Rect.h"
#include "Color.h"

namespace ncine {

/// A scene node representing a basic sprite
class DLL_PUBLIC Sprite : public DrawableNode
{
  public:
	/// Constructor for a sprite with a parent and texture, positioned in the relative origin
	Sprite(SceneNode* parent, Texture *texture);
	/// Constructor for a sprite with a texture but no parent, positioned in the origin
	explicit Sprite(Texture *texture);
	/// Constructor for a sprite with a parent, a texture and a specified relative position
	Sprite(SceneNode* parent, Texture *texture, float x, float y);
	/// Constructor for a sprite with a texture and a specified position but no parent
	Sprite(Texture *texture, float x, float y);

	/// Returns sprite width
	inline float width() const { return width_ * scaleFactor_; }
	/// Returns sprite height
	inline float height() const { return height_ * scaleFactor_; }
	/// Returns sprite size
	inline Vector2f size() const { return Vector2f(width(), height()); }
	/// Returns sprite rectangle
	Rectf rect() const;

	/// Sets sprite width
	inline void setWidth(float width) { width_ = width; }
	/// Sets sprite height
	inline void setHeight(float height) { height_ = height; }
	/// Sets sprite size
	inline void setSize(const Vector2f &size)
	{
		width_ = size.x;
		height_ = size.y;
	}

	/// Returns sprite absolute width
	inline float absWidth() const { return width_ * absScaleFactor_; }
	/// Returns sprite absolute height
	inline float absHeight() const { return height_ * absScaleFactor_; }
	/// Returns sprite absolute size
	inline Vector2f absSize() const { return Vector2f(absWidth(), absHeight()); }

	/// Gets the texture object
	inline const Texture* texture() const { return texture_; }
	/// Sets the texture object
	inline void setTexture(Texture *texture) { texture_ = texture; }
	/// Sets a flag that makes a transparent texture to be considered opaque
	/*! \note This flag comes in handy when the sprite uses an opaque region of a transparent texture atlas. */
	inline void setOpaqueTexture(bool opaqueTexture) { opaqueTexture_ = opaqueTexture; }

	/// Gets the texture source rectangle for blitting
	inline Recti texRect() const { return texRect_; }
	/// Ses the texture source rectangle for blitting
	void setTexRect(const Recti& rect);

	/// Flips the texture rect along the X coordinate
	void flipX();
	/// Flips the texture rect along the Y coordinate
	void flipY();

	inline static ObjectType sType() { return SPRITE_TYPE; }

  private:
	/// The sprite texture
	Texture *texture_;
	/// The opaque texture flag
	bool opaqueTexture_;
	/// The texture source rectangle
	Recti texRect_;
	/// Sprite width in pixel
	float width_;
	/// Sprite height in pixel
	float height_;

	/// Common sprite initialization for every constructor
	void init();
	virtual void updateRenderCommand();
};

inline Rectf Sprite::rect() const
{
	float halfAbsW = width_ * absScaleFactor_ * 0.5f;
	float halfAbsH = height_ * absScaleFactor_ * 0.5f;
	return Rectf(x - halfAbsW, y - halfAbsH, halfAbsW * 2.0f, halfAbsH * 2.0f);
}

inline void Sprite::setTexRect(const Recti& rect)
{
	texRect_ = rect;
	height_ = static_cast<float>(rect.h);
	width_ = static_cast<float>(rect.w);
}

inline void Sprite::flipX()
{
	texRect_.x += texRect_.w;
	texRect_.w *= -1;
}

inline void Sprite::flipY()
{
	texRect_.y += texRect_.h;
	texRect_.h *= -1;
}

}

#endif
