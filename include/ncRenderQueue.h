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
		: m_uNumVertices(0), m_uLastNumVertices(0), m_uLastNumCommands(0),
		  m_opaqueRenderCmds(16), m_transparentRenderCmds(16) { }
	~ncRenderQueue() { }

	// Adds a draw command to the queue
	void AddCommand(const ncRenderCommand *pCommand);
	// Sorts the queue then issues every render command in order
	void Draw();

	/// Returns the total number of vertices to be rendered by the queue
	inline unsigned int NumVertices() const { return m_uLastNumVertices; }
	/// Returns the queue's length
	inline unsigned int NumCommands() const { return m_uLastNumCommands; }

	/// Returns the total number of vertices to be rendered by the queue for a specified command type
	inline unsigned int NumVertices(ncRenderCommand::eCommandType eType) const { return m_uTypedLastNumVertices[eType]; }
	/// Returns the number of commands to render a specified category of commands
	inline unsigned int NumCommands(ncRenderCommand::eCommandType eType) const { return m_uTypedLastNumCommands[eType]; }

 private:
	/// The current sum of vertices for every command in the queue
	unsigned int m_uNumVertices;
	/// The sum of vertices in the previous frame (it never contains a partial sum)
	unsigned int m_uLastNumVertices;
	/// The sum of draw commands in the previous frame (it never contains a partial sum)
	unsigned int m_uLastNumCommands;

	/// The current sum of vertices for a specified command type
	unsigned int m_uTypedNumVertices[ncRenderCommand::TYPE_COUNT];
	/// The current sum of draw commands for a specified command type
	unsigned int m_uTypedNumCommands[ncRenderCommand::TYPE_COUNT];
	/// The sum of vertices for a specified command type in the previous frame (it never contains a partial sum)
	unsigned int m_uTypedLastNumVertices[ncRenderCommand::TYPE_COUNT];
	/// The sum of draw commands for a specified command type in the previous frame (it never contains a partial sum)
	unsigned int m_uTypedLastNumCommands[ncRenderCommand::TYPE_COUNT];

	/// Array of opaque render command pointers
	ncArray<const ncRenderCommand *> m_opaqueRenderCmds;
	/// Array of transparent render command pointers
	ncArray<const ncRenderCommand *> m_transparentRenderCmds;

	// Sorts render nodes in both queues to minimize state changes
	void SortQueues();

	void QSort(ncArray<const ncRenderCommand *> &array, int start, int end);
	int QSortPartition(ncArray<const ncRenderCommand *> &array, int start, int end);
};

#endif
