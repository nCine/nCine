#include "Object.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Object::Object(ObjectType type) : type_(type), id_(0), name_(MaxNameLength)
{
	id_ = theServiceLocator().indexer().addObject(this);
}

Object::Object(ObjectType type, const char *name) : type_(type), id_(0), name_(name)
{
	id_ = theServiceLocator().indexer().addObject(this);
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
