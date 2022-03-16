#ifndef CLASS_NCINE_GLSHADERATTRIBUTES
#define CLASS_NCINE_GLSHADERATTRIBUTES

#include <nctl/StaticHashMap.h>
#include <nctl/String.h>
#include "GLVertexFormat.h"

namespace ncine {

class GLShaderProgram;
class GLBufferObject;

/// A class to handle all the attributes of a shader program using a hashmap
class GLShaderAttributes
{
  public:
	GLShaderAttributes();
	explicit GLShaderAttributes(GLShaderProgram *shaderProgram);
	void setProgram(GLShaderProgram *shaderProgram);

	inline unsigned int numAttributes() const { return attributeLocations_.size(); }
	inline bool hasAttribute(const char *name) const { return (attributeLocations_.find(name) != nullptr); }
	GLVertexFormat::Attribute *attribute(const char *name);
	inline void defineVertexFormat(const GLBufferObject *vbo) { defineVertexFormat(vbo, nullptr, 0); }
	inline void defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo) { defineVertexFormat(vbo, ibo, 0); }
	void defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset);

  private:
	GLShaderProgram *shaderProgram_;

	nctl::StaticHashMap<nctl::String, int, GLVertexFormat::MaxAttributes> attributeLocations_;
	GLVertexFormat vertexFormat_;

	void importAttributes();
};

}

#endif
