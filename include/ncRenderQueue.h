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
	/// Array of render command pointers
	ncArray<const ncRenderCommand *> m_renderCmds;

	// Sorts render nodes to minimize state changes
	void SortQueue();

	void QSort(ncArray<const ncRenderCommand *> &array, int start, int end);
	int QSortPartition(ncArray<const ncRenderCommand *> &array, int start, int end);

public:
	ncRenderQueue() : m_renderCmds(16) { }
	~ncRenderQueue() { }

	/// Adds a draw command to the queue
	void AddCommand(const ncRenderCommand *pCommand) { m_renderCmds.InsertBack(pCommand); }
	// Sorts the queue then issues every render command in order
	void Draw();
};

#endif
