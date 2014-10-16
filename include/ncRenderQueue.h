#ifndef CLASS_NCRENDERQUEUE
#define CLASS_NCRENDERQUEUE

#include "ncRenderCommand.h"
#include "ncArray.h"
class ncSceneNode;

/// Traverse and flatten the scenegraph representation
/** This class is responsible for traversing the scenegraph
and creating a tree of render commands */
class ncRenderQueue
{
  public:
	ncRenderQueue()
		: numVertices_(0), lastNumVertices_(0), lastNumCommands_(0),
		  opaqueRenderCommands_(16), transparentRenderCommands_(16) { }
	~ncRenderQueue() { }

	// Adds a draw command to the queue
	void addCommand(const ncRenderCommand *command);
	// Sorts the queue then issues every render command in order
	void draw();

	/// Returns the total number of vertices to be rendered by the queue
	inline unsigned int numVertices() const { return lastNumVertices_; }
	/// Returns the queue's length
	inline unsigned int numCommands() const { return lastNumCommands_; }

	/// Returns the total number of vertices to be rendered by the queue for a specified command type
	inline unsigned int numVertices(ncRenderCommand::CommandType type) const { return typedLastNumVertices_[type]; }
	/// Returns the number of commands to render a specified category of commands
	inline unsigned int numCommands(ncRenderCommand::CommandType type) const { return typedLastNumCommands_[type]; }

  private:
	/// The current sum of vertices for every command in the queue
	unsigned int numVertices_;
	/// The sum of vertices in the previous frame (it never contains a partial sum)
	unsigned int lastNumVertices_;
	/// The sum of draw commands in the previous frame (it never contains a partial sum)
	unsigned int lastNumCommands_;

	/// The current sum of vertices for a specified command type
	unsigned int typedNumVertices_[ncRenderCommand::TYPE_COUNT];
	/// The current sum of draw commands for a specified command type
	unsigned int typedNumCommands_[ncRenderCommand::TYPE_COUNT];
	/// The sum of vertices for a specified command type in the previous frame (it never contains a partial sum)
	unsigned int typedLastNumVertices_[ncRenderCommand::TYPE_COUNT];
	/// The sum of draw commands for a specified command type in the previous frame (it never contains a partial sum)
	unsigned int typedLastNumCommands_[ncRenderCommand::TYPE_COUNT];

	/// Array of opaque render command pointers
	ncArray<const ncRenderCommand *> opaqueRenderCommands_;
	/// Array of transparent render command pointers
	ncArray<const ncRenderCommand *> transparentRenderCommands_;

	// Sorts render nodes in both queues to minimize state changes
	void sortQueues();

	void qSort(ncArray<const ncRenderCommand *> &array, int start, int end);
	int qSortPartition(ncArray<const ncRenderCommand *> &array, int start, int end);
};

#endif
