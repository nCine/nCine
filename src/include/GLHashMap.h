#ifndef CLASS_NCINE_GLHASHMAP
#define CLASS_NCINE_GLHASHMAP

#include <cstdlib> // for exit()
#include "ServiceLocator.h"

namespace ncine {

typedef GLenum key_t;
typedef GLuint value_t;

/// Naive implementation of a hashmap for storing pairs of OpenGL targets and object ids
template<unsigned int S, class MappingFunc>
class GLHashMap
{
  public:
	GLHashMap();
	value_t& operator[](key_t key);

  private:
	value_t buckets_[S];
	MappingFunc mappingFunc;
};

template<unsigned int S, class MappingFunc>
GLHashMap<S, MappingFunc>::GLHashMap()
{
	for (unsigned int i = 0; i < S; i++)
	{
		// Initializing with a null OpenGL object id
		buckets_[i] = 0;
	}
}

template<unsigned int S, class MappingFunc>
inline value_t& GLHashMap<S, MappingFunc>::operator[](key_t key)
{
	return buckets_[mappingFunc(key)];
}

class GLBufferObjectMappingFunc
{
  public:
	static const unsigned int Size = 2;
	inline unsigned int operator()(key_t key) const
	{
		switch (key)
		{
			case GL_ARRAY_BUFFER:
				return 0;
				break;
			case GL_ELEMENT_ARRAY_BUFFER:
				return 1;
				break;
			default:
				LOGE_X("No available case to handle key: %u", key);
				exit(EXIT_FAILURE);
				break;
		}
	}
};

class GLFramebufferMappingFunc
{
  public:
	static const unsigned int Size = 1;
	inline unsigned int operator()(key_t key) const
	{
		switch (key)
		{
			case GL_FRAMEBUFFER:
				return 0;
				break;
			default:
				LOGE_X("No available case to handle key: %u", key);
				exit(EXIT_FAILURE);
				break;
		}
	}
};

class GLTextureMappingFunc
{
  public:
	static const unsigned int Size = 3;
	inline unsigned int operator()(key_t key) const
	{
		switch (key)
		{
#ifndef __ANDROID__ // not available in OpenGL ES
			case GL_TEXTURE_1D:
				return 0;
				break;
#endif
			case GL_TEXTURE_2D:
				return 1;
				break;
#ifndef __ANDROID__ // available in OpenGL ES 3.0
			case GL_TEXTURE_3D:
				return 2;
				break;
#endif
			default:
				LOGE_X("No available case to handle key: %u", key);
				exit(EXIT_FAILURE);
				break;
		}
	}
};

}

#endif
