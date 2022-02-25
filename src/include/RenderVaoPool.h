#ifndef CLASS_NCINE_RENDERVAOPOOL
#define CLASS_NCINE_RENDERVAOPOOL

#include <nctl/Array.h>
#include <nctl/UniquePtr.h>
#include "TimeStamp.h"
#include "GLVertexArrayObject.h"
#include "GLVertexFormat.h"

namespace ncine {

class GLVertexArrayObject;

/// The class that creates and handles the pool of VAOs
class RenderVaoPool
{
  public:
	explicit RenderVaoPool(unsigned int vaoPoolSize);
	void bindVao(const GLVertexFormat &vertexFormat);

  private:
	struct VaoBinding
	{
		nctl::UniquePtr<GLVertexArrayObject> object;
		GLVertexFormat format;
		TimeStamp lastBindTime;
	};

	nctl::Array<VaoBinding> vaoPool_;

	void insertGLDebugMessage(const VaoBinding &binding);
};

}

#endif
