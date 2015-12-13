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

	virtual bool isEmpty() const { return numObjects_ == 0; }
	virtual unsigned int size() const { return numObjects_; }

	virtual void logReport() const;

  private:
	unsigned int numObjects_;
	unsigned int nextId_;
	Array<Object*> pointers_;

	/// Private copy constructor
	ArrayIndexer(const ArrayIndexer&);
	/// Private assignment operator
	ArrayIndexer& operator=(const ArrayIndexer&);
};

}

#endif
