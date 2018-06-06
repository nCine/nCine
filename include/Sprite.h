#ifndef CLASS_NCINE_SPRITE
#define CLASS_NCINE_SPRITE

#include "BaseSprite.h"

namespace ncine {

/// A scene node representing a regular sprite
class DLL_PUBLIC Sprite : public BaseSprite
{
  public:
	/// Constructor for a sprite with a parent and texture, positioned in the relative origin
	Sprite(SceneNode *parent, Texture *texture);
	/// Constructor for a sprite with a texture but no parent, positioned in the origin
	explicit Sprite(Texture *texture);
	/// Constructor for a sprite with a parent, a texture and a specified relative position
	Sprite(SceneNode *parent, Texture *texture, float x, float y);
	/// Constructor for a sprite with a texture and a specified position but no parent
	Sprite(Texture *texture, float x, float y);

	inline static ObjectType sType() { return ObjectType::SPRITE; }
};

}

#endif
