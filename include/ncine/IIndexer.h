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
	virtual bool removeObject(unsigned int id) = 0;

	/// Returns the object with the specified object id, if any
	virtual Object *object(unsigned int id) const = 0;
	/// Sets the object pointer for the specified id
	virtual bool setObject(unsigned int id, Object *object) = 0;

	/// Returns true if the index is empty
	virtual bool isEmpty() const = 0;

	/// Returns the number of objects in the index
	virtual unsigned int size() const = 0;

	/// Prints in the log a list of currently indexed objects
	virtual void logReport() const = 0;
};

inline IIndexer::~IIndexer() {}

/// Fake indexer, always returning `nullptr` and a zero index
class DLL_PUBLIC NullIndexer : public IIndexer
{
  public:
	unsigned int addObject(Object *object) override { return 0U; }
	bool removeObject(unsigned int id) override { return true; }

	Object *object(unsigned int id) const override { return nullptr; }
	bool setObject(unsigned int id, Object *object) override { return true; };

	bool isEmpty() const override { return true; }

	unsigned int size() const override { return 0U; }

	void logReport() const override {}
};

}

#endif
