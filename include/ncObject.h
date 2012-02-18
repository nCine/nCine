#ifndef CLASS_NCOBJECT
#define CLASS_NCOBJECT

#include <cstring>
#include "ncServiceLocator.h"

/// Maximum length for an object name
static const int NC_OBJNAME_LENGTH = 128;

/// Static RRTI and identification index
class ncObject
{
private:
	/// Object identification in the indexer
	unsigned int m_uId;

	/// Object name
	/** This field is currently only useful in debug,
	as there's still no string hashing based search. */
	char m_cName[NC_OBJNAME_LENGTH];

protected:
	/// The enumeration of object types
	enum eObjectType {
		BASE_TYPE = 0,
		TEXTURE_TYPE,
		SCENENODE_TYPE,
		SPRITE_TYPE,
		SPRITEBATCH_TYPE,
		PARTICLESYSTEM_TYPE,
		TEXT_TYPE,
		ANIMATEDSPRITE_TYPE,
		AUDIOBUFFER_TYPE,
		AUDIOBUFFERPLAYER_TYPE,
		AUDIOSTREAMPLAYER_TYPE
	};

	/// Object type
	eObjectType m_eType;

public:
	ncObject() : m_uId(0), m_eType(BASE_TYPE)
	{
		memset(m_cName, 0, NC_OBJNAME_LENGTH);
		m_uId = ncServiceLocator::Indexer().AddObject(this);
	}
	virtual ~ncObject() { ncServiceLocator::Indexer().RemoveObject(m_uId); }

	/// Returns the object identification number
	inline unsigned int Id() const { return m_uId; }

	/// Returns the object type (RTTI)
	eObjectType Type() const { return m_eType; }
	/// Static method to return class type
	inline static eObjectType sType() { return BASE_TYPE; }

	/// Returns object name
	char const * const Name() const { return m_cName; }
	/// Sets the object name
	void SetName(const char cName[NC_OBJNAME_LENGTH]) { strncpy(m_cName, cName, NC_OBJNAME_LENGTH); }
};

#endif
