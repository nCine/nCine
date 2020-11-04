#ifndef CLASS_NCINE_OBJECT
#define CLASS_NCINE_OBJECT

#include <nctl/String.h>

namespace ncine {

/// Static RRTI and identification index
class DLL_PUBLIC Object
{
  public:
	/// Object types
	enum class ObjectType
	{
		BASE = 0,
		TEXTURE,
		SCENENODE,
		SPRITE,
		MESH_SPRITE,
		ANIMATED_SPRITE,
		PARTICLE_SYSTEM,
		FONT,
		TEXTNODE,
		AUDIOBUFFER,
		AUDIOBUFFER_PLAYER,
		AUDIOSTREAM_PLAYER
	};

	/// Maximum length for an object name
	static const unsigned int MaxNameLength = 256;

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
	inline static ObjectType sType() { return ObjectType::BASE; }

	/// Returns the object name
	inline const nctl::String &name() const { return name_; }
	/// Sets the object name
	inline void setName(const char *name) { name_ = name; }

	/// Returns a casted pointer to the object with the specified id, if any exists
	template <class T>
	static T *fromId(unsigned int id);

  protected:
	/// Object type
	ObjectType type_;

  private:
	/// Object identification in the indexer
	unsigned int id_;

	/// Object name
	/*! \note This field is currently only useful in debug,
	 *  as there is still no string hashing based search. */
	nctl::String name_;

	/// Deleted copy constructor
	Object(const Object &) = delete;
	/// Deleted assignment operator
	Object &operator=(const Object &) = delete;
};

}

#endif
