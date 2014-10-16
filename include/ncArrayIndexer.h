#ifndef CLASS_NCARRAYINDEXER
#define CLASS_NCARRAYINDEXER

#include "ncIIndexer.h"
#include "ncArray.h"
#include "ncObject.h"

/// Keeps track of allocated ncObjects in a growing only array
class ncArrayIndexer : public ncIIndexer
{
  public:
	ncArrayIndexer();
	~ncArrayIndexer();

	virtual unsigned int addObject(ncObject* object);
	virtual void removeObject(unsigned int id);

	virtual ncObject* object(unsigned int id) const;

	virtual bool isEmpty() const;

  private:
	unsigned int nextId_;
	ncArray<ncObject*> pointers_;
};

inline unsigned int ncArrayIndexer::addObject(ncObject* object)
{
	pointers_[nextId_] = object;
	nextId_++;

	return nextId_ - 1;
}
inline void ncArrayIndexer::removeObject(unsigned int id)
{
	// setting to NULL instead of physically removing
	if (id < pointers_.size())
	{
		pointers_[id] = NULL;
	}
}

inline ncObject *ncArrayIndexer::object(unsigned int id) const
{
	if (id < pointers_.size())
	{
		return pointers_[id];
	}
	else
	{
		return NULL;
	}
}

inline bool ncArrayIndexer::isEmpty() const
{
	bool isEmpty = true;

	for (unsigned int i = 0; i < pointers_.size(); i++)
	{
		if (pointers_[i])
		{
			// There is at least one pointer stored
			isEmpty = false;
			break;
		}
	}

	return isEmpty;
}

#endif
