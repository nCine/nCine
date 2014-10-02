#ifndef CLASS_NCOBJECT
#define CLASS_NCOBJECT

#include <cstring>
#include "ncServiceLocator.h"

/// Static RRTI and identification index
class ncObject
{
  public:
	/// The enumeration of object types
	enum eObjectType
	{
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

	/// Maximum length for an object name
	static const int s_iNameLength = 128;

	ncObject() : m_eType(BASE_TYPE), m_uId(0)
	{
		memset(m_vName, 0, s_iNameLength);
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
	char const * const Name() const { return m_vName; }
	/// Sets the object name
	inline void SetName(const char vName[s_iNameLength]) { strncpy(m_vName, vName, s_iNameLength); }

	// Returns a casted pointer to the object with the specified id, if any exists
	template <class T> static T* FromId(unsigned int uId);

  protected:
	/// Object type
	eObjectType m_eType;

  private:
	/// Object identification in the indexer
	unsigned int m_uId;

	/// Object name
	/** This field is currently only useful in debug,
	as there's still no string hashing based search. */
	char m_vName[s_iNameLength];
};

/// Returns a casted pointer to the object with the specified id, if any exists
template <class T>
T* ncObject::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::Indexer().Object(uId);

	if (pObject)
	{
		if (pObject->m_eType == T::sType())
		{
			return static_cast<T *>(pObject);
		}
		else // Cannot cast
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncObject::FromId - Object \"%s\" (%u) is of type %u instead of %u", pObject->m_vName, uId, pObject->m_eType, T::sType());
			return NULL;
		}
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncObject::FromId - Object %u not found", uId);
		return NULL;
	}
}

#endif
