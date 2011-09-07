#ifndef CLASS_NCRENDERQUEUE
#define CLASS_NCRENDERQUEUE

#include "ncRenderCommand.h"
#include "ncArray.h"
class ncSceneNode;

/// Traverse and flatten the scenegraph representation
/*! This class is responsible for traversing the scenegraph
 * and creating a tree of render commands
 */
class ncRenderQueue
{
private:
	// add the array of sortkey/rendercmd ptrs
	ncArray<const ncRenderCommand *> m_renderCmds;

	void ProcessNode(ncSceneNode& rNode);
	void SortQueue();

	void QSort(ncArray<const ncRenderCommand *> &array, int start, int end);
	int QSortPartition(ncArray<const ncRenderCommand *> &array, int start, int end);

public:
	ncRenderQueue() : m_renderCmds(16) { }
	~ncRenderQueue() { }

	void Traverse(ncSceneNode& rNode);
	void Draw();
};

#endif
