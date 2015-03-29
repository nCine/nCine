#ifndef CLASS_NCINE_GLHASHMAP
#define CLASS_NCINE_GLHASHMAP

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

class GLHashMapMappingFunc
{
  public:
	static const unsigned int Size = 1;
	inline unsigned int operator()(key_t key) const
	{
		switch(key)
		{
			default:
			case GL_ARRAY_BUFFER:
				return 0;
		}
	}
};

class GLBufferObjectMappingFunc
{
  public:
	static const unsigned int Size = 1;
	inline unsigned int operator()(key_t key) const
	{
		switch(key)
		{
			default:
			case GL_ARRAY_BUFFER:
				return 0;
		}
	}
};

class GLFramebufferMappingFunc
{
  public:
	static const unsigned int Size = 1;
	inline unsigned int operator()(key_t key) const
	{
		switch(key)
		{
			default:
			case GL_FRAMEBUFFER:
				return 0;
		}
	}
};

class GLTextureMappingFunc
{
  public:
	static const unsigned int Size = 3;
	inline unsigned int operator()(key_t key) const
	{
		switch(key)
		{
			default:
			case GL_TEXTURE_1D:
				return 0;
			case GL_TEXTURE_2D:
				return 1;
			case GL_TEXTURE_3D:
				return 2;
		}
	}
};

}

#endif
