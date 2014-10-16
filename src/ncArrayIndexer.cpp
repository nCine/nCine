#include "ncArrayIndexer.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncArrayIndexer::ncArrayIndexer()
	: nextId_(0), pointers_(16)
{
	// First element reserved
	pointers_.insertBack(NULL);
	nextId_++;
}

ncArrayIndexer::~ncArrayIndexer()
{
	for (unsigned int i = 0; i < pointers_.size(); i++)
	{
		if (pointers_[i])
		{
			delete pointers_[i];
			pointers_[i] = NULL;
		}
	}

}
