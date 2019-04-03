#ifndef CLASS_NCINE_DRAWABLENODE
#define CLASS_NCINE_DRAWABLENODE

#include "SceneNode.h"
#include "Rect.h"

namespace ncine {

class RenderCommand;
class RenderQueue;

/// A class for objects that can be drawn through the render queue
class DLL_PUBLIC DrawableNode : public SceneNode
{
  public:
	/// Drawing layers from back to front
	struct LayerBase
	{
		enum
		{
			LOWEST = 0,
			SCENE = 0,
			HUD = 32767,
			HIGHEST = 65535
		};
	};

	/// Constructor for a drawable node with a parent and a specified relative position
	DrawableNode(SceneNode *parent, float xx, float yy);
	/// Constructor for a drawable node with a parent and a specified relative position as a vector
	DrawableNode(SceneNode *parent, const Vector2f &position);
	/// Constructor for a drawable node with a parent and positioned in the relative origin
	explicit DrawableNode(SceneNode *parent);
	/// Constructor for a drawable node with no parent and positioned in the origin
	DrawableNode();

	/// Updates the draw command and adds it to the queue
	void draw(RenderQueue &renderQueue) override;

	/// Returns the width of the node area
	inline virtual float width() const { return width_ * scaleFactor_; }
	/// Returns the height of the node area
	inline virtual float height() const { return height_ * scaleFactor_; }
	/// Returns the size of the node area
	inline Vector2f size() const { return Vector2f(width(), height()); }

	/// Returns the absolute width of the node area
	inline virtual float absWidth() const { return width_ * absScaleFactor_; }
	/// Returns the absolute height of the node area
	inline virtual float absHeight() const { return height_ * absScaleFactor_; }
	/// Returns the absolute size of the node area
	inline Vector2f absSize() const { return Vector2f(absWidth(), absHeight()); }

	/// Returns the node rendering layer
	unsigned int layer() const;
	/// Sets the node rendering layer
	void setLayer(unsigned int layer);

  protected:
	/// Node width in pixel
	float width_;
	/// Node height in pixel
	float height_;

	/// The render command class associated with this node
	nctl::UniquePtr<RenderCommand> renderCommand_;

	/// Axis Aligned Bounding Box of the node area
	Rectf aabb_;
	/// Calculates updated values for the AABB
	virtual void updateAabb();

	/// Updates the render command
	virtual void updateRenderCommand() = 0;

  private:
	/// Deleted copy constructor
	DrawableNode(const DrawableNode &) = delete;
	/// Deleted assignment operator
	DrawableNode &operator=(const DrawableNode &) = delete;
};

}

#endif
