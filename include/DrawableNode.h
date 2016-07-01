#ifndef CLASS_NCINE_DRAWABLENODE
#define CLASS_NCINE_DRAWABLENODE

#include "SceneNode.h"

namespace ncine {

class RenderCommand;
class RenderQueue;

/// A class for objects that can be drawn through the render queue
class DLL_PUBLIC DrawableNode : public SceneNode
{
  public:
	/// Drawing layers from back to front
	enum LayerBase
	{
		SCENE_LAYER = 0,
		HUD_LAYER = 32767
	};

	DrawableNode(SceneNode* parent, float x, float y);
	explicit DrawableNode(SceneNode* parent);
	DrawableNode();
	virtual ~DrawableNode();

	// Updates the draw command and adds it to the queue
	virtual void draw(RenderQueue& renderQueue);

	// Returns the node rendering layer
	unsigned int layer() const;
	// Sets the node rendering layer
	void setLayer(unsigned int layer);

  protected:
	/// The render command class associated with this node
	RenderCommand* renderCommand_;

	/// Updates the render command
	virtual void updateRenderCommand() = 0;

  private:
	/// Private copy constructor
	DrawableNode(const DrawableNode&);
	/// Private assignment operator
	DrawableNode& operator=(const DrawableNode&);
};

}

#endif
