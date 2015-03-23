#ifndef CLASS_NCINE_IINDEXER
#define CLASS_NCINE_IINDEXER

#include <cstdio> // for NULL
#include "common_defines.h"

namespace ncine {

class Object;

/// The Interface for every Object indexer
class DLL_PUBLIC IIndexer
{
  public:
	virtual ~IIndexer() = 0;

	/// Adds an object to the index
	virtual unsigned int addObject(Object* object) = 0;
	/// Removes an object from the index
	virtual void removeObject(unsigned int id) = 0;

	/// Returns the object with the specified object id, if any
	virtual Object* object(unsigned int id) const = 0;

	/// Returns true if the index is empty
	virtual bool isEmpty() const = 0;
};

inline IIndexer::~IIndexer() { }

#endif

#ifndef CLASS_NCINE_NULLINDEXER
#define CLASS_NCINE_NULLINDEXER

/// Fake indexer, always returning a NULL object and a zero index
class DLL_PUBLIC NullIndexer : public IIndexer
{
  public:
	virtual unsigned int addObject(Object* object) { return 0; }
	virtual void removeObject(unsigned int id) { }

	virtual Object* object(unsigned int id) const { return NULL; }

	virtual bool isEmpty() const { return true; }
};

}

#endif
