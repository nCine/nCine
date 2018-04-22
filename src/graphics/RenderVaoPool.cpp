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
			binding.object->bind();
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

		vaoPool_[index].format = vertexFormat;
		vaoPool_[index].object->bind();
		vaoPool_[index].format.define();
		vaoPool_[index].lastBindTime = Timer::now();
		RenderStatistics::addVaoPoolBinding();
	}
	GLDebug::popGroup();

	RenderStatistics::gatherVaoPoolStatistics(vaoPool_.size(), vaoPool_.capacity());
}

}
