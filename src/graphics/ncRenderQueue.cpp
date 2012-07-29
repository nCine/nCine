#include "ncRenderQueue.h"
#include "ncSceneNode.h"
#include "ncArray.h"
#include "ncSprite.h"

#ifdef WITH_DEPTH_TEST
	#if defined(__ANDROID__)
		#include <GLES/gl.h>
		#include <GLES/glext.h>
	#elif defined(WITH_GLEW)
		#include <GL/glew.h>
	#elif defined(__APPLE__)
		#include <OpenGL/gl.h>
		#include <OpenGL/glext
	#else
		#include <GL/gl.h>
		#include <GL/glext.h>
	#endif
#endif

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

#ifdef WITH_DEPTH_TEST
	if (pCommand->Material().isTransparent() == false)
		m_opaqueRenderCmds.InsertBack(pCommand);
	else
#endif
		m_transparentRenderCmds.InsertBack(pCommand);
}

/// Sorts the queue then issues every render command in order
void ncRenderQueue::Draw()
{
	SortQueues();

#ifdef WITH_DEPTH_TEST
	glDisable(GL_BLEND);
	// TODO: Investigate about the heavy performance drop with alpha testing
	glEnable(GL_ALPHA_TEST);
	// Rendering opaque nodes front to back
	for (int i = m_opaqueRenderCmds.Size()-1; i > -1; i--)
		m_opaqueRenderCmds[i]->Issue();

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
#endif
	// Rendering transparent nodes back to front
	for (unsigned int i = 0; i < m_transparentRenderCmds.Size(); i++)
		m_transparentRenderCmds[i]->Issue();
#ifdef WITH_DEPTH_TEST
	// Has to be enabled again before exiting this method
	// or glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
	glDepthMask(GL_TRUE);
#endif

	m_uLastNumVertices = m_uNumVertices;
	m_uLastNumCommands = m_opaqueRenderCmds.Size() + m_transparentRenderCmds.Size();
	m_uNumVertices = 0;

	for (int i = 0; i < ncRenderCommand::TYPE_COUNT; i++)
	{
		m_uTypedLastNumVertices[i] = m_uTypedNumVertices[i];
		m_uTypedLastNumCommands[i] = m_uTypedNumCommands[i];
		m_uTypedNumVertices[i] = 0;
		m_uTypedNumCommands[i] = 0;
	}

	m_opaqueRenderCmds.Clear();
	m_transparentRenderCmds.Clear();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Sorts render nodes in both queues to minimize state changes
void ncRenderQueue::SortQueues()
{
	QSort(m_opaqueRenderCmds, 0, m_opaqueRenderCmds.Size()-1);
	QSort(m_transparentRenderCmds, 0, m_transparentRenderCmds.Size()-1);

	// Check sorting correctness
//	for (int i = 1; i < m_opaqueRenderCmds.Size(); i++)
//		assert(m_opaqueRenderCmds[i-1]->SortKey() <= m_opaqueRenderCmds[i]->SortKey());
//	for (int i = 1; i < m_transparentRenderCmds.Size(); i++)
//		assert(m_transparentRenderCmds[i-1]->SortKey() <= m_transparentRenderCmds[i]->SortKey());
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
