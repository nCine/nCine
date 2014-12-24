#include "ArrayIndexer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ArrayIndexer::ArrayIndexer()
	: nextId_(0), pointers_(16)
{
	// First element reserved
	pointers_.insertBack(NULL);
	nextId_++;
}

ArrayIndexer::~ArrayIndexer()
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

}
