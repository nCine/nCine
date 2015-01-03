#ifndef CLASS_NCINE_SPRITE
#define CLASS_NCINE_SPRITE

#include "DrawableNode.h"
#include "Texture.h"
#include "Rect.h"
#include "Color.h"
#include "StaticArray.h"

namespace ncine {

class SpriteBatchNode;

/// A scene node representing a basic sprite
class Sprite : public DrawableNode
{
  public:
	Sprite(SceneNode* parent, Texture *texture);
	explicit Sprite(Texture *texture);
	Sprite(SceneNode* parent, Texture *texture, float x, float y);
	Sprite(Texture *texture, float x, float y);

	/// Returns sprite width
	inline int width() const { return width_; }
	/// Returns sprite height
	inline int height() const { return height_; }
	/// Returns sprite size
	inline Point size() const	{ return Point(width_, height_); }
	// Returns sprite rectangle
	Rect rect() const;

	/// Sets sprite width
	inline void setWidth(int width) { width_ = width; }
	/// Sets sprite height
	inline void setHeight(int height) { height_ = height; }
	/// Sets sprite size
	inline void setSize(const Point &size)
	{
		width_ = size.x;
		height_ = size.y;
	}

	/// Returns sprite absolute width
	inline int absWidth() const { return width_ * absScaleFactor_; }
	/// Returns sprite absolute height
	inline int absHeight() const { return height_ * absScaleFactor_; }
	/// Returns sprite absolute size
	inline Point absSize() const
	{
		return Point(width_ * absScaleFactor_, height_ * absScaleFactor_);
	}

	/// Gets the texture object
	inline const Texture* texture() const { return texture_; }
	/// Sets the texture object
	inline void setTexture(Texture *texture) { texture_ = texture; }

	/// Gets the texture source rectangle for blitting
	inline Rect texRect() const { return texRect_; }
	// Ses the texture source rectangle for blitting
	void setTexRect(const Rect& rect);

	// Flips the texture rect along the X coordinate
	void flipX();
	// Flips the texture rect along the Y coordinate
	void flipY();

	inline static ObjectType sType() { return SPRITE_TYPE; }

  private:
	/// The sprite texture
	Texture *texture_;
	/// Used as source rectangle by the sprite batch class
	Rect texRect_;
	/// Sprite width in pixel
	int width_;
	/// Sprite height in pixel
	int height_;

	StaticArray<float, 8> vertices_;
	StaticArray<float, 8> texCoords_;

	void init();
	void setVertices();
	void setTexCoords();

	virtual void updateRenderCommand();

	friend class SpriteBatchNode;
};

/// Returns sprite rectangle
inline Rect Sprite::rect() const
{
	float halfAbsW = width_ * absScaleFactor_ * 0.5f;
	float halfAbsH = height_ * absScaleFactor_ * 0.5f;
	return Rect(x - halfAbsW, y - halfAbsH, halfAbsW * 2.0f, halfAbsH * 2.0f);
}

/// Sets the texture source rectangle for blitting
inline void Sprite::setTexRect(const Rect& rect)
{
	texRect_ = rect;
	height_ = rect.h;
	width_ = rect.w;
	setTexCoords();
}

/// Flips the texture rect along the X coordinate
inline void Sprite::flipX()
{
	texRect_.x += texRect_.w;
	texRect_.w *= -1;
	setTexCoords();
}

/// Flips the texture rect along the Y coordinate
inline void Sprite::flipY()
{
	texRect_.y += texRect_.h;
	texRect_.h *= -1;
	setTexCoords();
}

}

#endif
