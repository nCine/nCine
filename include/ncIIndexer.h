#ifndef CLASS_NCIINDEXER
#define CLASS_NCIINDEXER

#include <cstdio> // for NULL

class ncObject;

/// The Interface for every ncObject indexer
class ncIIndexer
{
public:
	virtual ~ncIIndexer() = 0;

	virtual unsigned int AddObject(ncObject* pObject) = 0;
	virtual void RemoveObject(unsigned int uId) = 0;

	virtual ncObject* Object(unsigned int uId) const = 0;

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
	virtual unsigned int AddObject(ncObject* pObject) { return 0; }
	virtual void RemoveObject(unsigned int uId) { }

	virtual ncObject* Object(unsigned int uId) const { return NULL; }

	virtual bool isEmpty() const { return true; }
};

#endif
