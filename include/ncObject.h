#ifndef CLASS_NCOBJECT
#define CLASS_NCOBJECT

#include <cstring>
#include "ncServiceLocator.h"

/// Static RRTI and identification index
class ncObject
{
  public:
	/// The enumeration of object types
	enum ObjectType
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
	static const int MaxNameLength = 128;

	ncObject() : type_(BASE_TYPE), id_(0)
	{
		memset(name_, 0, MaxNameLength);
		id_ = ncServiceLocator::indexer().addObject(this);
	}
	virtual ~ncObject() { ncServiceLocator::indexer().removeObject(id_); }

	/// Returns the object identification number
	inline unsigned int id() const { return id_; }

	/// Returns the object type (RTTI)
	ObjectType type() const { return type_; }
	/// Static method to return class type
	inline static ObjectType sType() { return BASE_TYPE; }

	/// Returns object name
	char const * const name() const { return name_; }
	/// Sets the object name
	inline void setName(const char name[MaxNameLength]) { strncpy(name_, name, MaxNameLength); }

	// Returns a casted pointer to the object with the specified id, if any exists
	template <class T> static T* fromId(unsigned int id);

  protected:
	/// Object type
	ObjectType type_;

  private:
	/// Object identification in the indexer
	unsigned int id_;

	/// Object name
	/** This field is currently only useful in debug,
	as there's still no string hashing based search. */
	char name_[MaxNameLength];
};

/// Returns a casted pointer to the object with the specified id, if any exists
template <class T>
T* ncObject::fromId(unsigned int id)
{
	ncObject *object = ncServiceLocator::indexer().object(id);

	if (object)
	{
		if (object->type_ == T::sType())
		{
			return static_cast<T *>(object);
		}
		else // Cannot cast
		{
			LOGF_X("Object \"%s\" (%u) is of type %u instead of %u", object->name_, id, object->type_, T::sType());
			return NULL;
		}
	}
	else
	{
		LOGW_X("Object %u not found", id);
		return NULL;
	}
}

#endif
