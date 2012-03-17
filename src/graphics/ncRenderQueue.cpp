#include "ncRenderQueue.h"
#include "ncSceneNode.h"
#include "ncArray.h"
#include "ncSprite.h"

//#include <assert.h> // for checking sorting correctness

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a draw command to the queue
void ncRenderQueue::AddCommand(const ncRenderCommand *pCommand)
{
	m_uTypedNumVertices[pCommand->Type()] += pCommand->Geometry().NumVertices();
	m_uTypedNumCommands[pCommand->Type()]++;

	m_uNumVertices += pCommand->Geometry().NumVertices();
	m_renderCmds.InsertBack(pCommand);
}

/// Sorts the queue then issues every render command in order
void ncRenderQueue::Draw()
{
	SortQueue();

	for (int i = 0; i < m_renderCmds.Size(); i++)
		m_renderCmds[i]->Issue();

	m_uLastNumVertices = m_uNumVertices;
	m_uLastNumCommands = m_renderCmds.Size();
	m_uNumVertices = 0;

	for (int i = 0; i < ncRenderCommand::TYPE_COUNT; i++)
	{
		m_uTypedLastNumVertices[i] = m_uTypedNumVertices[i];
		m_uTypedLastNumCommands[i] = m_uTypedNumCommands[i];
		m_uTypedNumVertices[i] = 0;
		m_uTypedNumCommands[i] = 0;
	}

	m_renderCmds.Clear();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Sorts render nodes to minimize state changes
void ncRenderQueue::SortQueue()
{
	QSort(m_renderCmds, 0, m_renderCmds.Size()-1);

	// Check sorting correctness
//	for (int i = 1; i < m_renderCmds.Size(); i++)
//		assert(m_renderCmds[i-1]->SortKey() <= m_renderCmds[i]->SortKey());
}

void ncRenderQueue::QSort(ncArray<const ncRenderCommand *> &array, int start, int end)
{
	if (start < end) {
		int div = QSortPartition(array, start, end);
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
