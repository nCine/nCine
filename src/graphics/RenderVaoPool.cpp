#include "RenderVaoPool.h"
#include "GLVertexArrayObject.h"
#include "RenderStatistics.h"
#include "Timer.h"
#include "GLDebug.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderVaoPool::RenderVaoPool(unsigned int vaoPoolSize)
    : vaoPool_(vaoPoolSize, nctl::ArrayMode::FIXED_CAPACITY)
{
	// Start with a VAO bound to the OpenGL context
	GLVertexFormat format;
	bindVao(format);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderVaoPool::bindVao(const GLVertexFormat &vertexFormat)
{
	bool vaoFound = false;
	for (VaoBinding &binding : vaoPool_)
	{
		if (binding.format == vertexFormat)
		{
			GLDebug::pushGroup("Bind VAO");
			vaoFound = true;
			const bool bindChanged = binding.object->bind();
			const GLuint iboHandle = vertexFormat.ibo() ? vertexFormat.ibo()->glHandle() : 0;
			if (bindChanged)
			{
				// Binding a VAO changes the current bound element array buffer
				GLBufferObject::setBoundHandle(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
			}
			else
			{
				// The VAO was already bound but it is not known if the bound element array buffer changed in the meantime
				GLBufferObject::bindHandle(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
			}
			binding.lastBindTime = Timer::now();
			RenderStatistics::addVaoPoolBinding();
			break;
		}
	}

	if (vaoFound == false)
	{
		unsigned int index = 0;
		if (vaoPool_.size() < vaoPool_.capacity())
		{
			GLDebug::pushGroup("Create and define VAO");
			index = vaoPool_.size();
			vaoPool_[index].object = nctl::makeUnique<GLVertexArrayObject>();
		}
		else
		{
			// Find the least recently used VAO
			float time = vaoPool_[0].lastBindTime;
			for (unsigned int i = 1; i < vaoPool_.size(); i++)
			{
				if (vaoPool_[i].lastBindTime < time)
				{
					index = i;
					time = vaoPool_[i].lastBindTime;
				}
			}

			GLDebug::pushGroup("Reuse and define VAO");
			RenderStatistics::addVaoPoolReuse();
		}

		const bool bindChanged = vaoPool_[index].object->bind();
		ASSERT(bindChanged == true);
		// Binding a VAO changes the current bound element array buffer
		const GLuint oldIboHandle = vaoPool_[index].format.ibo() ? vaoPool_[index].format.ibo()->glHandle() : 0;
		GLBufferObject::setBoundHandle(GL_ELEMENT_ARRAY_BUFFER, oldIboHandle);
		vaoPool_[index].format = vertexFormat;
		vaoPool_[index].format.define();
		vaoPool_[index].lastBindTime = Timer::now();
		RenderStatistics::addVaoPoolBinding();
	}
	GLDebug::popGroup();

	RenderStatistics::gatherVaoPoolStatistics(vaoPool_.size(), vaoPool_.capacity());
}

}
