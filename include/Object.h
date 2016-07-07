#ifndef CLASS_NCINE_OBJECT
#define CLASS_NCINE_OBJECT

#include "ncString.h"

namespace ncine {

/// Static RRTI and identification index
class DLL_PUBLIC Object
{
  public:
	/// Object types
	enum ObjectType
	{
		BASE_TYPE = 0,
		TEXTURE_TYPE,
		SCENENODE_TYPE,
		SPRITE_TYPE,
		PARTICLESYSTEM_TYPE,
		FONT_TYPE,
		TEXTNODE_TYPE,
		ANIMATEDSPRITE_TYPE,
		AUDIOBUFFER_TYPE,
		AUDIOBUFFERPLAYER_TYPE,
		AUDIOSTREAMPLAYER_TYPE
	};

	/// Maximum length for an object name
	static const unsigned int MaxNameLength = 128;

	/// Constructs an object with a specified type and adds it to the index
	explicit Object(ObjectType type);
	/// Constructs an object with a specified type and name and adds it to the index
	Object(ObjectType type, const char *name);
	/// Removes an object from the index and then destroys it
	virtual ~Object();

	/// Returns the object identification number
	inline unsigned int id() const { return id_; }

	/// Returns the object type (RTTI)
	inline ObjectType type() const { return type_; }
	/// Static method to return class type
	inline static ObjectType sType() { return BASE_TYPE; }

	/// Returns the object name
	inline const String& name() const { return name_; }
	/// Sets the object name
	inline void setName(const String &name) { name_ = name; }

	/// Returns a casted pointer to the object with the specified id, if any exists
	template <class T> static T* fromId(unsigned int id);

  protected:
	/// Object type
	ObjectType type_;

  private:
	/// Object identification in the indexer
	unsigned int id_;

	/// Object name
	/*! This field is currently only useful in debug,
	 *  as there is still no string hashing based search. */
	String name_;

	/// Private copy constructor
	Object(const Object&);
	/// Private assignment operator
	Object& operator=(const Object&);
};

}

#endif
