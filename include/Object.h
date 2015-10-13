#ifndef CLASS_NCINE_OBJECT
#define CLASS_NCINE_OBJECT

#include "ncString.h"
#include "ServiceLocator.h"

namespace ncine {

/// Static RRTI and identification index
class DLL_PUBLIC Object
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
	static const unsigned int MaxNameLength = 128;

	Object() : type_(BASE_TYPE), id_(0), name_(MaxNameLength)
	{
		id_ = theServiceLocator().indexer().addObject(this);
	}
	virtual ~Object() { theServiceLocator().indexer().removeObject(id_); }

	/// Returns the object identification number
	inline unsigned int id() const { return id_; }

	/// Returns the object type (RTTI)
	ObjectType type() const { return type_; }
	/// Static method to return class type
	inline static ObjectType sType() { return BASE_TYPE; }

	/// Returns object name
	const String& name() const { return name_; }
	/// Sets the object name
	void setName(const String &name) { name_ = name; }

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
	String name_;

	/// Private copy constructor
	Object(const Object&);
	/// Private assignment operator
	Object& operator=(const Object&);
};

/// Returns a casted pointer to the object with the specified id, if any exists
template <class T>
T* Object::fromId(unsigned int id)
{
	Object *object = theServiceLocator().indexer().object(id);

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

}

#endif
