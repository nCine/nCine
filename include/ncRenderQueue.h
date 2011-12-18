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
private:
	/// The current sum of vertices for every command in the queue
	unsigned int m_uNumVertices;
	/// The sum of vertices in the previous frame (it never contains a partial sum)
	unsigned int m_uLastNumVertices;
	/// The sum of draw commands in the previous frame (it never contains a partial sum)
	unsigned int m_uLastNumCommands;
	/// Array of render command pointers
	ncArray<const ncRenderCommand *> m_renderCmds;

	// Sorts render nodes to minimize state changes
	void SortQueue();

	void QSort(ncArray<const ncRenderCommand *> &array, int start, int end);
	int QSortPartition(ncArray<const ncRenderCommand *> &array, int start, int end);

public:
	ncRenderQueue() : m_uNumVertices(0), m_renderCmds(16) { }
	~ncRenderQueue() { }

	// Adds a draw command to the queue
	void AddCommand(const ncRenderCommand *pCommand);
	// Sorts the queue then issues every render command in order
	void Draw();

	/// Returns the total number of vertices to be rendered by the queue
	inline unsigned int NumVertices() const { return m_uLastNumVertices; }
	/// Returns the queue's length
	inline unsigned int NumCommands() const { return m_uLastNumCommands; }
};

#endif
