#include <nctl/StaticString.h>
#include "RenderVaoPool.h"
#include "GLVertexArrayObject.h"
#include "RenderStatistics.h"
#include "GLDebug.h"

namespace ncine {

namespace {
	/// The string used to output OpenGL debug group information
	static nctl::StaticString<128> debugString;
}

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
			vaoFound = true;
			const bool bindChanged = binding.object->bind();
			const GLuint iboHandle = vertexFormat.ibo() ? vertexFormat.ibo()->glHandle() : 0;
			if (bindChanged)
			{
				if (GLDebug::isAvailable())
					insertGLDebugMessage(binding);

				// Binding a VAO changes the current bound element array buffer
				GLBufferObject::setBoundHandle(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
			}
			else
			{
				// The VAO was already bound but it is not known if the bound element array buffer changed in the meantime
				GLBufferObject::bindHandle(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
			}
			binding.lastBindTime = TimeStamp::now();
			RenderStatistics::addVaoPoolBinding();
			break;
		}
	}

	if (vaoFound == false)
	{
		unsigned int index = 0;
		if (vaoPool_.size() < vaoPool_.capacity())
		{
			vaoPool_.emplaceBack();
			vaoPool_.back().object = nctl::makeUnique<GLVertexArrayObject>();
			index = vaoPool_.size() - 1;

			if (GLDebug::isAvailable())
			{
				debugString.format("Created and defined VAO 0x%lx (%u)", uintptr_t(vaoPool_[index].object.get()), index);
				GLDebug::messageInsert(debugString.data());

				debugString.format("VAO_#%d", index);
				vaoPool_.back().object->setObjectLabel(debugString.data());
			}
		}
		else
		{
			// Find the least recently used VAO
			TimeStamp time = vaoPool_[0].lastBindTime;
			for (unsigned int i = 1; i < vaoPool_.size(); i++)
			{
				if (vaoPool_[i].lastBindTime < time)
				{
					index = i;
					time = vaoPool_[i].lastBindTime;
				}
			}

			debugString.format("Reuse and define VAO 0x%lx (%u)", uintptr_t(vaoPool_[index].object.get()), index);
			GLDebug::messageInsert(debugString.data());
			RenderStatistics::addVaoPoolReuse();
		}

		const bool bindChanged = vaoPool_[index].object->bind();
		ASSERT(bindChanged == true || vaoPool_.size() == 1);
		// Binding a VAO changes the current bound element array buffer
		const GLuint oldIboHandle = vaoPool_[index].format.ibo() ? vaoPool_[index].format.ibo()->glHandle() : 0;
		GLBufferObject::setBoundHandle(GL_ELEMENT_ARRAY_BUFFER, oldIboHandle);
		vaoPool_[index].format = vertexFormat;
		vaoPool_[index].format.define();
		vaoPool_[index].lastBindTime = TimeStamp::now();
		RenderStatistics::addVaoPoolBinding();
	}

	RenderStatistics::gatherVaoPoolStatistics(vaoPool_.size(), vaoPool_.capacity());
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderVaoPool::insertGLDebugMessage(const VaoBinding &binding)
{
	debugString.format("Bind VAO 0x%lx (", uintptr_t(binding.object.get()));
	bool firstVbo = true;
	for (unsigned int i = 0; i < binding.format.numAttributes(); i++)
	{
		if (binding.format[i].isEnabled() && binding.format[i].vbo() != nullptr)
		{
			if (firstVbo == false)
				debugString.formatAppend(", ");
			debugString.formatAppend("vbo #%u: 0x%lx", i, uintptr_t(binding.format[i].vbo()));
			firstVbo = false;
		}
	}
	if (binding.format.ibo() != nullptr)
		debugString.formatAppend(", ibo: 0x%lx", uintptr_t(binding.format.ibo()));
	debugString.formatAppend(")");

	GLDebug::messageInsert(debugString.data());
}

}
