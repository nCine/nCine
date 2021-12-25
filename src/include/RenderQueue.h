#ifndef CLASS_NCINE_RENDERQUEUE
#define CLASS_NCINE_RENDERQUEUE

#include "RenderCommand.h"
#include <nctl/Array.h>

namespace ncine {

class Viewport;

/// A class that sorts and issues the render commands collected by the scenegraph visit
class RenderQueue
{
  public:
	/// Constructor that sets the owning viewport
	explicit RenderQueue(Viewport &viewport);

	/// Adds a draw command to the queue
	void addCommand(RenderCommand *command);

	/// Sorts the queues, create batches and commits commands
	void sortAndCommit();
	/// Issues every render command in order
	void draw();

	/// Returns the viewport that owns this render queue
	inline Viewport &viewport() const { return viewport_; }

  private:
	/// The string used to output OpenGL debug group information
	nctl::String debugGroupString_;

	/// The viewport that owns this render queue
	Viewport &viewport_;

	/// Array of opaque render command pointers
	nctl::Array<RenderCommand *> opaqueQueue_;
	/// Array of opaque batched render command pointers
	nctl::Array<RenderCommand *> opaqueBatchedQueue_;
	/// Array of transparent render command pointers
	nctl::Array<RenderCommand *> transparentQueue_;
	/// Array of transparent batched render command pointers
	nctl::Array<RenderCommand *> transparentBatchedQueue_;
};

}

#endif
