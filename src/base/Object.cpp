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
	// Don't remove the object id from indexer if this is a moved out object
	if (id_ > 0)
		theServiceLocator().indexer().removeObject(id_);
}

Object::Object(Object &&other)
    : type_(other.type_), id_(other.id_), name_(nctl::move(other.name_))
{
	theServiceLocator().indexer().setObject(id_, this);
	other.id_ = 0;
}

Object &Object::operator=(Object &&other)
{
	type_ = other.type_;
	theServiceLocator().indexer().removeObject(id_);
	id_ = other.id_;
	name_ = nctl::move(other.name_);

	other.id_ = 0;
	return *this;
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

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

Object::Object(const Object &other)
    : type_(other.type_), id_(0), name_(other.name_)
{
	id_ = theServiceLocator().indexer().addObject(this);
}

}
