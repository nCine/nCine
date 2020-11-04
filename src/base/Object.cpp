#include "common_macros.h"
#include "Object.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Object::Object(ObjectType type)
    : type_(type), id_(0), name_(MaxNameLength)
{
	id_ = theServiceLocator().indexer().addObject(this);
}

Object::Object(ObjectType type, const char *name)
    : type_(type), id_(0), name_(MaxNameLength)
{
	id_ = theServiceLocator().indexer().addObject(this);
	name_ = name;
}

Object::~Object()
{
	theServiceLocator().indexer().removeObject(id_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
T *Object::fromId(unsigned int id)
{
	const Object *object = theServiceLocator().indexer().object(id);

	if (object)
	{
		if (object->type_ == T::sType())
			return static_cast<T *>(object);
		else // Cannot cast
		{
			LOGF_X("Object \"%s\" (%u) is of type %u instead of %u", object->name_, id, object->type_, T::sType());
			return nullptr;
		}
	}
	else
	{
		LOGW_X("Object %u not found", id);
		return nullptr;
	}
}

}
