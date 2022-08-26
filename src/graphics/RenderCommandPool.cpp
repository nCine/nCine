#include "RenderCommandPool.h"
#include "RenderCommand.h"
#include "RenderStatistics.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommandPool::RenderCommandPool(unsigned int poolSize)
    : freeCommandsPool_(poolSize), usedCommandsPool_(poolSize)
{
}

RenderCommandPool::~RenderCommandPool() = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

RenderCommand *RenderCommandPool::add()
{
	nctl::UniquePtr<RenderCommand> newCommand = nctl::makeUnique<RenderCommand>();
	usedCommandsPool_.pushBack(nctl::move(newCommand));

	return usedCommandsPool_.back().get();
}

RenderCommand *RenderCommandPool::add(GLShaderProgram *shaderProgram)
{
	RenderCommand *newCommand = add();
	newCommand->material().setShaderProgram(shaderProgram);

	return newCommand;
}

RenderCommand *RenderCommandPool::retrieve(GLShaderProgram *shaderProgram)
{
	RenderCommand *retrievedCommand = nullptr;

	for (unsigned int i = 0; i < freeCommandsPool_.size(); i++)
	{
		const unsigned int poolSize = freeCommandsPool_.size();
		nctl::UniquePtr<RenderCommand> &command = freeCommandsPool_[i];
		if (command && command->material().shaderProgram() == shaderProgram)
		{
			retrievedCommand = command.get();
			usedCommandsPool_.pushBack(nctl::move(command));
			command = nctl::move(freeCommandsPool_[poolSize - 1]);
			freeCommandsPool_.popBack();
			break;
		}
	}

	if (retrievedCommand)
		RenderStatistics::addCommandPoolRetrieval();

	return retrievedCommand;
}

RenderCommand *RenderCommandPool::retrieveOrAdd(GLShaderProgram *shaderProgram, bool &commandAdded)
{
	RenderCommand *retrievedCommand = retrieve(shaderProgram);

	commandAdded = false;
	if (retrievedCommand == nullptr)
	{
		retrievedCommand = add(shaderProgram);
		commandAdded = true;
	}

	return retrievedCommand;
}

void RenderCommandPool::reset()
{
	RenderStatistics::gatherCommandPoolStatistics(usedCommandsPool_.size(), freeCommandsPool_.size());

	for (nctl::UniquePtr<RenderCommand> &command : usedCommandsPool_)
		freeCommandsPool_.pushBack(nctl::move(command));
	usedCommandsPool_.clear();
}

}
