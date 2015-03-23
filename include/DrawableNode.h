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
	/// Drawing priorities from back to front
	enum PriorityBase
	{
		SCENE_PRIORITY = 0,
		HUD_PRIORITY = 128
	};

	DrawableNode(SceneNode* parent, float x, float y);
	DrawableNode(SceneNode* parent);
	DrawableNode();
	virtual ~DrawableNode();

	// Updates the draw command and adds it to the queue
	virtual void draw(RenderQueue& renderQueue);

	// Applies node transformations to vertices
	void applyTransformations();

	// Returns the node rendering priority
	int priority() const;
	// Sets the node rendering priority
	void setPriority(int priority);

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
