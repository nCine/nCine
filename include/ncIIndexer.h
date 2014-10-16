#ifndef CLASS_NCIINDEXER
#define CLASS_NCIINDEXER

#include <cstdio> // for NULL

class ncObject;

/// The Interface for every ncObject indexer
class ncIIndexer
{
  public:
	virtual ~ncIIndexer() = 0;

	/// Adds an object to the index
	virtual unsigned int addObject(ncObject* object) = 0;
	/// Removes an object from the index
	virtual void removeObject(unsigned int id) = 0;

	/// Returns the object with the specified object id, if any
	virtual ncObject* object(unsigned int id) const = 0;

	/// Returns true if the index is empty
	virtual bool isEmpty() const = 0;
};

inline ncIIndexer::~ncIIndexer() { }

#endif

#ifndef CLASS_NCNULLINDEXER
#define CLASS_NCNULLINDEXER

/// Fake indexer, always returning a NULL object and a zero index
class ncNullIndexer : public ncIIndexer
{
  public:
	virtual unsigned int addObject(ncObject* object) { return 0; }
	virtual void removeObject(unsigned int id) { }

	virtual ncObject* object(unsigned int id) const { return NULL; }

	virtual bool isEmpty() const { return true; }
};

#endif
