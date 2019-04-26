#ifndef CLASS_NCINE_RENDERVAOPOOL
#define CLASS_NCINE_RENDERVAOPOOL

#include <nctl/Array.h>
#include <nctl/UniquePtr.h>
#include "GLVertexArrayObject.h"
#include "GLVertexFormat.h"

namespace ncine {

class GLVertexArrayObject;

/// The class that creates and handles the pool of VAOs
class RenderVaoPool
{
  public:
	RenderVaoPool(unsigned int vaoPoolSize);
	void bindVao(const GLVertexFormat &vertexFormat);

  private:
	struct VaoBinding
	{
		VaoBinding()
		    : lastBindTime(0) {}

		nctl::UniquePtr<GLVertexArrayObject> object;
		GLVertexFormat format;
		float lastBindTime;
	};

	nctl::Array<VaoBinding> vaoPool_;
};

}

#endif
