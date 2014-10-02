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

	virtual unsigned int AddObject(ncObject* pObject);
	virtual void RemoveObject(unsigned int uId);

	virtual ncObject* Object(unsigned int uId) const;

	virtual bool isEmpty() const;

  private:
	unsigned int m_uNextId;
	ncArray<ncObject*> m_vPointers;
};

inline unsigned int ncArrayIndexer::AddObject(ncObject* pObject)
{
	m_vPointers[m_uNextId] = pObject;
	m_uNextId++;

	return m_uNextId - 1;
}
inline void ncArrayIndexer::RemoveObject(unsigned int uId)
{
	// setting to NULL instead of physically removing
	if (uId < m_vPointers.Size())
	{
		m_vPointers[uId] = NULL;
	}
}

inline ncObject *ncArrayIndexer::Object(unsigned int uId) const
{
	if (uId < m_vPointers.Size())
	{
		return m_vPointers[uId];
	}
	else
	{
		return NULL;
	}
}

inline bool ncArrayIndexer::isEmpty() const
{
	bool bEmpty = true;

	for (unsigned int i = 0; i < m_vPointers.Size(); i++)
	{
		if (m_vPointers[i])
		{
			// There is at least one pointer stored
			bEmpty = false;
			break;
		}
	}

	return bEmpty;
}

#endif
