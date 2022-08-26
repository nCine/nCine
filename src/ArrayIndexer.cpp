#include "ArrayIndexer.h"

namespace ncine {

const char *objectTypeToString(Object::ObjectType type)
{
	// clang-format off
	switch (type)
	{
		case Object::ObjectType::BASE:					return "Base";
		case Object::ObjectType::TEXTURE:				return "Texture";
		case Object::ObjectType::SHADER:				return "Shader";
		case Object::ObjectType::SCENENODE:				return "SceneNode";
		case Object::ObjectType::SPRITE:				return "Sprite";
		case Object::ObjectType::MESH_SPRITE:			return "MeshSprite";
		case Object::ObjectType::ANIMATED_SPRITE:		return "AnimatedSprite";
		case Object::ObjectType::PARTICLE_SYSTEM:		return "ParticleSystem";
		case Object::ObjectType::FONT:					return "Font";
		case Object::ObjectType::TEXTNODE:				return "TextNode";
		case Object::ObjectType::AUDIOBUFFER:			return "AudioBuffer";
		case Object::ObjectType::AUDIOBUFFER_PLAYER:	return "AudioBufferPlayer";
		case Object::ObjectType::AUDIOSTREAM_PLAYER:	return "AudioStreamPlayer";
		default:										return "Unknown";
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
			const char *objName = objPtr->name();

			if (objName)
				LOGI_X("%s object (id %u, 0x%x): \"%s\"", objectTypeToString(objPtr->type()), objPtr->id(), pointers_[i], objName);
			else
				LOGI_X("%s object (id %u, 0x%x)", objectTypeToString(objPtr->type()), objPtr->id(), pointers_[i]);
		}
	}
}

}
