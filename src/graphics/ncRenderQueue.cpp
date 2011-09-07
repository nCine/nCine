#include "ncRenderQueue.h"
#include "ncSceneNode.h"
#include "ncArray.h"
#include "ncSprite.h"

//#include <assert.h>

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncRenderQueue::Traverse(ncSceneNode& rNode)
{
	if(!rNode.bShouldDraw)
		return;

	ProcessNode(rNode);

	if(!rNode.Children().isEmpty())
	{
		const ncListNode<ncSceneNode *> *pListNode = rNode.Children().Head();

		while (pListNode)
		{
			// List nodes contain pointers, they need deferencing for Traverse()
			ncSceneNode& rChildNode = *(pListNode->Data());
			Traverse(rChildNode);
			pListNode = pListNode->Next();
		}
	}
}

void ncRenderQueue::Draw()
{
	SortQueue();

	for (int i = 0; i < m_renderCmds.Size(); i++)
		m_renderCmds[i]->Issue();

	m_renderCmds.Clear();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Process a node for subsequent drawing
void ncRenderQueue::ProcessNode(ncSceneNode& rNode)
{
	// Calculating absolute positions
	if (rNode.ParentNode())
	{
		const ncSceneNode& rParent = *(rNode.ParentNode());

		rNode.m_absX = rParent.m_absX + rNode.x;
		rNode.m_absY = rParent.m_absY + rNode.y;
	}
	else
	{
		rNode.m_absX = rNode.x;
		rNode.m_absY = rNode.y;
	}


	if (rNode.Type() == ncSprite::sType())
	{
		ncSprite& sprite = static_cast<ncSprite &>(rNode);

		sprite.UpdateRenderCommand();
		m_renderCmds.InsertBack(sprite.Command());
	}
}

/// Sort render nodes to minimize state changes
void ncRenderQueue::SortQueue()
{
	QSort(m_renderCmds, 0, m_renderCmds.Size()-1);

	// Check sorting correctness
//	for (int i = 1; i < m_renderCmds.Size(); i++)
//		assert(m_renderCmds[i-1]->SortKey() <= m_renderCmds[i]->SortKey());
}

void ncRenderQueue::QSort(ncArray<const ncRenderCommand *> &array, int start, int end)
{
	int div;

	if (start < end) {
		div = QSortPartition(array, start, end);
		QSort(array, start, div);
		QSort(array, div+1, end);
	}
}

int ncRenderQueue::QSortPartition(ncArray<const ncRenderCommand *> &array, int start, int end)
{
	bool bShouldQuit;
	int i, j;
	unsigned long int pivot;

	pivot = array[start]->SortKey();
	i = start - 1;
	j = end + 1;

	bShouldQuit = false;

	do {
		do {
			j--;
		} while(array[j]->SortKey() > pivot);

		do {
			i++;
		} while(array[i]->SortKey() < pivot);

		if (i < j) {
			const ncRenderCommand *pTemp = array[i];
			array[i] = array[j];
			array[j] = pTemp;
		}
		else
			bShouldQuit = true;
	} while(!bShouldQuit);

	return j;
}
