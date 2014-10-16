#ifndef CLASS_NCSPRITE
#define CLASS_NCSPRITE

#include "ncDrawableNode.h"
#include "ncTexture.h"
#include "ncRect.h"
#include "ncColor.h"
class ncSpriteBatchNode;

/// A scene node representing a basic sprite
class ncSprite : public ncDrawableNode
{
  public:
	ncSprite(ncSceneNode* parent, ncTexture *texture);
	ncSprite(ncTexture *texture);
	ncSprite(ncSceneNode* parent, ncTexture *texture, int x, int y);
	ncSprite(ncTexture *texture, int x, int y);
	virtual ~ncSprite() { }

	/// Returns sprite width
	inline int width() const { return width_; }
	/// Returns sprite height
	inline int height() const { return height_; }
	/// Returns sprite size
	inline ncPoint size() const	{ return ncPoint(width_, height_); }
	// Returns sprite rectangle
	ncRect rect() const;

	/// Sets sprite width
	inline void setWidth(int width) { width_ = width; }
	/// Sets sprite height
	inline void setHeight(int height) { height_ = height; }
	/// Sets sprite size
	inline void setSize(const ncPoint &size)
	{
		width_ = size.x;
		height_ = size.y;
	}

	/// Returns sprite absolute width
	inline int absWidth() const { return width_ * absScaleFactor_; }
	/// Returns sprite absolute height
	inline int absHeight() const { return height_ * absScaleFactor_; }
	/// Returns sprite absolute size
	inline ncPoint absSize() const
	{
		return ncPoint(width_ * absScaleFactor_, height_ * absScaleFactor_);
	}

	/// Gets the texture object
	inline const ncTexture* texture() const { return texture_; }
	/// Sets the texture object
	inline void setTexture(ncTexture *texture) { texture_ = texture; }

	/// Gets the texture source rectangle for blitting
	inline ncRect texRect() const { return texRect_; }
	// Ses the texture source rectangle for blitting
	void setTexRect(const ncRect& rect);

	// Flips the texture rect along the X coordinate
	void flipX();
	// Flips the texture rect along the Y coordinate
	void flipY();

	inline static ObjectType sType() { return SPRITE_TYPE; }

  private:
	/// The sprite texture
	ncTexture *texture_;
	/// Used as source rectangle by the sprite batch class
	ncRect texRect_;
	/// Sprite width in pixel
	int width_;
	/// Sprite height in pixel
	int height_;

	float vertices_[8];
	float texCoords_[8];

	void init();
	void setVertices();
	void setTexCoords();

	virtual void updateRenderCommand();

	friend class ncSpriteBatchNode;
};

/// Returns sprite rectangle
inline ncRect ncSprite::rect() const
{
	float halfAbsW = width_ * absScaleFactor_ * 0.5f;
	float halfAbsH = height_ * absScaleFactor_ * 0.5f;
	return ncRect(x - halfAbsW, y - halfAbsH, halfAbsW * 2.0f, halfAbsH * 2.0f);
}

/// Sets the texture source rectangle for blitting
inline void ncSprite::setTexRect(const ncRect& rect)
{
	texRect_ = rect;
	height_ = rect.h;
	width_ = rect.w;
	setTexCoords();
}

/// Flips the texture rect along the X coordinate
inline void ncSprite::flipX()
{
	texRect_.x += texRect_.w;
	texRect_.w *= -1;
	setTexCoords();
}

/// Flips the texture rect along the Y coordinate
inline void ncSprite::flipY()
{
	texRect_.y += texRect_.h;
	texRect_.h *= -1;
	setTexCoords();
}

#endif
