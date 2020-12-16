#include "ArrayIndexer.h"

namespace ncine {

const char *objectTypeToString(Object::ObjectType type)
{
	// clang-format off
	switch (type)
	{
		case Object::ObjectType::BASE:					return "Base"; break;
		case Object::ObjectType::TEXTURE:				return "Texture"; break;
		case Object::ObjectType::SCENENODE:				return "SceneNode"; break;
		case Object::ObjectType::SPRITE:				return "Sprite"; break;
		case Object::ObjectType::MESH_SPRITE:			return "MeshSprite"; break;
		case Object::ObjectType::ANIMATED_SPRITE:		return "AnimatedSprite"; break;
		case Object::ObjectType::PARTICLE_SYSTEM:		return "ParticleSystem"; break;
		case Object::ObjectType::FONT:					return "Font"; break;
		case Object::ObjectType::TEXTNODE:				return "TextNode"; break;
		case Object::ObjectType::AUDIOBUFFER:			return "AudioBuffer"; break;
		case Object::ObjectType::AUDIOBUFFER_PLAYER:	return "AudioBufferPlayer"; break;
		case Object::ObjectType::AUDIOSTREAM_PLAYER:	return "AudioStreamPlayer"; break;
		default:										return "Unknown"; break;
	}
	// clang-format on
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ArrayIndexer::ArrayIndexer()
    : numObjects_(0), nextId_(0), pointers_(16)
{
	// First element reserved
	pointers_.pushBack(nullptr);
	nextId_++;
}

ArrayIndexer::~ArrayIndexer()
{
	for (Object *obj : pointers_)
		delete obj;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ArrayIndexer::addObject(Object *object)
{
	if (object == nullptr)
		return 0;

	numObjects_++;

	pointers_.pushBack(object);
	nextId_++;

	return nextId_ - 1;
}

bool ArrayIndexer::removeObject(unsigned int id)
{
	// setting to `nullptr` instead of physically removing
	if (id < pointers_.size() && pointers_[id] != nullptr)
	{
		pointers_[id] = nullptr;
		numObjects_--;
		return true;
	}
	return false;
}

Object *ArrayIndexer::object(unsigned int id) const
{
	Object *objPtr = nullptr;

	if (id < pointers_.size())
		objPtr = pointers_[id];

	return objPtr;
}

bool ArrayIndexer::setObject(unsigned int id, Object *object)
{
	if (id < pointers_.size())
	{
		pointers_[id] = object;
		return true;
	}
	return false;
}

void ArrayIndexer::logReport() const
{
	for (unsigned int i = 0; i < pointers_.size(); i++)
	{
		if (pointers_[i])
		{
			const Object *objPtr = object(i);
			LOGI_X("%s object (id %u, 0x%x): \"%s\"", objectTypeToString(objPtr->type()), objPtr->id(), pointers_[i], objPtr->name().data());
		}
	}
}

}
