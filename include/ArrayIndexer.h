#ifndef CLASS_NCINE_ARRAYINDEXER
#define CLASS_NCINE_ARRAYINDEXER

#include "IIndexer.h"
#include "Array.h"
#include "Object.h"

namespace ncine {

/// Keeps track of allocated objects in a growing only array
class ArrayIndexer : public IIndexer
{
  public:
	ArrayIndexer();
	~ArrayIndexer();

	virtual unsigned int addObject(Object* object);
	virtual void removeObject(unsigned int id);

	virtual Object* object(unsigned int id) const;

	virtual bool isEmpty() const;

  private:
	unsigned int nextId_;
	Array<Object*> pointers_;
};

inline unsigned int ArrayIndexer::addObject(Object* object)
{
	pointers_[nextId_] = object;
	nextId_++;

	return nextId_ - 1;
}
inline void ArrayIndexer::removeObject(unsigned int id)
{
	// setting to NULL instead of physically removing
	if (id < pointers_.size())
	{
		pointers_[id] = NULL;
	}
}

inline Object *ArrayIndexer::object(unsigned int id) const
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

inline bool ArrayIndexer::isEmpty() const
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

}

#endif
