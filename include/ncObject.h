#ifndef CLASS_NCOBJECT
#define CLASS_NCOBJECT

#define NC_OBJNAME_LENGTH 128
#include <cstring>

#include "ncServiceLocator.h"

/// Static RRTI and id index
class ncObject
{
private:
	/// Object identification in the indexer
	unsigned int m_uId;

	/// This field is currently only useful in debug,
	/// as there's still no string hashing based search.
	char m_cName[NC_OBJNAME_LENGTH];
protected:
	enum eObjectType {
		BASE_TYPE = 0,
		TEXTURE_TYPE,
		SCENENODE_TYPE,
		SPRITE_TYPE,
		SPRITEBATCH_TYPE,
		PARTICLESYSTEM_TYPE
	};

	eObjectType m_eType;


public:
	ncObject() : m_uId(0), m_eType(BASE_TYPE)
	{
		memset(m_cName, 0, NC_OBJNAME_LENGTH);
		m_uId = ncServiceLocator::GetIndexer().AddObject(this);
	}
	virtual ~ncObject() { ncServiceLocator::GetIndexer().RemoveObject(m_uId); }

	inline unsigned int Id() const { return m_uId; }

	eObjectType Type() const { return m_eType; }
	inline static eObjectType sType() { return BASE_TYPE; }

	char const * const Name() const { return m_cName; }
	void SetName(const char cName[NC_OBJNAME_LENGTH]) { strncpy(m_cName, cName, NC_OBJNAME_LENGTH); }
};

#endif
