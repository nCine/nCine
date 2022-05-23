#ifndef CLASS_NCINE_RENDERCOMMANDPOOL
#define CLASS_NCINE_RENDERCOMMANDPOOL

#include <nctl/Array.h>
#include <nctl/UniquePtr.h>
#include "Material.h"

namespace ncine {

class RenderCommand;

/// The class that creates and handles the pool of render commands
class RenderCommandPool
{
  public:
	RenderCommandPool(unsigned int poolSize);
	~RenderCommandPool();

	/// Adds a new command to the used pool and returns its pointer
	/*! \note It is usually called after checking that the retrieve method fails */
	RenderCommand *add();
	/// Adds a new command with the specified ShaderProgramType to the used pool and returns its pointer
	/*! \note It is usually called after checking that the retrieve method fails */
	RenderCommand *add(Material::ShaderProgramType shaderProgramType);

	/// Retrieves a command with the specified ShaderProgramType
	RenderCommand *retrieve(Material::ShaderProgramType shaderProgramType);
	/// Retrieves a command with the particular OpenGL shader program
	RenderCommand *retrieve(GLShaderProgram *shaderProgram);

	/// Retrieves (or adds) a command with the specified ShaderProgramType
	RenderCommand *retrieveOrAdd(Material::ShaderProgramType shaderProgramType, bool &commandAdded);

	/// Releases all used commands and returnsi them in the free array
	void reset();

  private:
	nctl::Array<nctl::UniquePtr<RenderCommand>> freeCommandsPool_;
	nctl::Array<nctl::UniquePtr<RenderCommand>> usedCommandsPool_;
};

}

#endif
