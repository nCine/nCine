#include "ArrayIndexer.h"

namespace ncine {

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

void ArrayIndexer::removeObject(unsigned int id)
{
	// setting to `nullptr` instead of physically removing
	if (id < pointers_.size() && pointers_[id] != nullptr)
	{
		pointers_[id] = nullptr;
		numObjects_--;
	}
}

Object *ArrayIndexer::object(unsigned int id) const
{
	Object *objPtr = nullptr;

	if (id < pointers_.size())
		objPtr = pointers_[id];

	return objPtr;
}

void ArrayIndexer::logReport() const
{
	for (unsigned int i = 0; i < pointers_.size(); i++)
	{
		if (pointers_[i])
		{
			Object *objPtr = object(i);

			nctl::String typeName(Object::MaxNameLength);
			// clang-format off
			switch (objPtr->type())
			{
				case Object::ObjectType::BASE:					typeName = "Base"; break;
				case Object::ObjectType::TEXTURE:				typeName = "Texture"; break;
				case Object::ObjectType::SCENENODE:				typeName = "SceneNode"; break;
				case Object::ObjectType::SPRITE:				typeName = "Sprite"; break;
				case Object::ObjectType::MESH_SPRITE:			typeName = "MeshSprite"; break;
				case Object::ObjectType::ANIMATED_SPRITE:		typeName = "AnimatedSprite"; break;
				case Object::ObjectType::PARTICLE_SYSTEM:		typeName = "ParticleSystem"; break;
				case Object::ObjectType::FONT:					typeName = "Font"; break;
				case Object::ObjectType::TEXTNODE:				typeName = "TextNode"; break;
				case Object::ObjectType::AUDIOBUFFER:			typeName = "AudioBuffer"; break;
				case Object::ObjectType::AUDIOBUFFER_PLAYER:	typeName = "AudioBufferPlayer"; break;
				case Object::ObjectType::AUDIOSTREAM_PLAYER:	typeName = "AudioStreamPlayer"; break;
				default:										typeName = "Unknown"; break;
			}
			// clang-format on

			LOGI_X("Object %u - type: %s - name: \"%s\"", objPtr->id(), typeName.data(), objPtr->name().data());
		}
	}
}

}
