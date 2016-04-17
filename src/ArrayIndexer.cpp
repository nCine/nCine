#include "ArrayIndexer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ArrayIndexer::ArrayIndexer()
	: numObjects_(0), nextId_(0), pointers_(16)
{
	// First element reserved
	pointers_.pushBack(NULL);
	nextId_++;
}

ArrayIndexer::~ArrayIndexer()
{
	for (unsigned int i = 0; i < pointers_.size(); i++)
	{
		if (pointers_[i])
		{
			delete pointers_[i];
			pointers_[i] = NULL;
		}
	}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ArrayIndexer::addObject(Object* object)
{
	if (object == NULL)
	{
		return 0;
	}

	numObjects_++;

	pointers_[nextId_] = object;
	nextId_++;

	return nextId_ - 1;
}

void ArrayIndexer::removeObject(unsigned int id)
{
	// setting to NULL instead of physically removing
	if (id < pointers_.size() && pointers_[id] != NULL)
	{
		pointers_[id] = NULL;
		numObjects_--;
	}
}

Object *ArrayIndexer::object(unsigned int id) const
{
	Object *objPtr = NULL;

	if (id < pointers_.size())
	{
		objPtr = pointers_[id];
	}

	return objPtr;
}

void ArrayIndexer::logReport() const
{
	for (unsigned int i = 0; i < pointers_.size(); i++)
	{
		if (pointers_[i])
		{
			Object *objPtr = object(i);

			String typeName(Object::MaxNameLength);
			switch (objPtr->type())
			{
				case Object::BASE_TYPE:					typeName = "Base"; break;
				case Object::TEXTURE_TYPE:				typeName = "Texture"; break;
				case Object::SCENENODE_TYPE:			typeName = "SceneNode"; break;
				case Object::SPRITE_TYPE:				typeName = "Sprite"; break;
				case Object::PARTICLESYSTEM_TYPE:		typeName = "ParticleSystem"; break;
				case Object::FONT_TYPE:					typeName = "Font"; break;
				case Object::TEXTNODE_TYPE:				typeName = "TextNode"; break;
				case Object::ANIMATEDSPRITE_TYPE:		typeName = "AnimatedSprite"; break;
				case Object::AUDIOBUFFER_TYPE:			typeName = "AudioBuffer"; break;
				case Object::AUDIOBUFFERPLAYER_TYPE:	typeName = "AudioBufferPlayer"; break;
				case Object::AUDIOSTREAMPLAYER_TYPE:	typeName = "AudioStreamPlayer"; break;
				default:								typeName = "Unknown"; break;
			}

			LOGI_X("Object %u - type: %s - name: \"%s\"", objPtr->id(), typeName.data(), objPtr->name().data());
		}
	}
}

}
