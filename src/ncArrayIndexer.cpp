#include "ncArrayIndexer.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncArrayIndexer::ncArrayIndexer()
	: m_vPointers(16), m_uNextId(0)
{
	// First element reserved
	m_vPointers.InsertBack(NULL);
	m_uNextId++;
}

ncArrayIndexer::~ncArrayIndexer()
{
	for(int i = 0; i < m_vPointers.Size(); i++)
	{
		if (m_vPointers[i])
		{
			delete m_vPointers[i];
			m_vPointers[i] = NULL;
		}
	}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////
