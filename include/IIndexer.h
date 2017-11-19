#ifndef CLASS_NCINE_IINDEXER
#define CLASS_NCINE_IINDEXER

#include "common_defines.h"

namespace ncine {

class Object;

/// The Interface for every Object indexer
class DLL_PUBLIC IIndexer
{
  public:
	virtual ~IIndexer() = 0;

	/// Adds an object to the index
	virtual unsigned int addObject(Object *object) = 0;
	/// Removes an object from the index
	virtual void removeObject(unsigned int id) = 0;

	/// Returns the object with the specified object id, if any
	virtual Object *object(unsigned int id) const = 0;

	/// Returns true if the index is empty
	virtual bool isEmpty() const = 0;

	/// Returns the number of objects in the index
	virtual unsigned int size() const = 0;

	/// Prints in the log a list of currently indexed objects
	virtual void logReport() const = 0;
};

inline IIndexer::~IIndexer() { }


/// Fake indexer, always returning `nullptr` and a zero index
class DLL_PUBLIC NullIndexer : public IIndexer
{
  public:
	virtual unsigned int addObject(Object *object) { return 0U; }
	virtual void removeObject(unsigned int id) { }

	virtual Object *object(unsigned int id) const { return nullptr; }

	virtual bool isEmpty() const { return true; }

	virtual unsigned int size() const { return 0U; }

	virtual void logReport() const { }
};

}

#endif
