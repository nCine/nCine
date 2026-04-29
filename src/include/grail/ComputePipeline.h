#ifndef CLASS_GRAIL_COMPUTEPIPELINE
#define CLASS_GRAIL_COMPUTEPIPELINE

#include <nctl/String.h>
#include "grail/pipeline.h"

namespace ncine {
namespace grail {

namespace ComputePipeline {

using pipeline::MaxBindGroups;
using pipeline::PipelineLayoutCreateDesc;
using pipeline::ShaderDesc;
using pipeline::PipelineLayoutDesc;

struct CreateDesc
{
	ShaderDesc computeShader;

	PipelineLayoutCreateDesc pipelineLayout;

	const char *debugName = nullptr;
};

struct Desc
{
	nctl::String computeShaderEntry;

	PipelineLayoutDesc pipelineLayout;
	BindGroupLayout::Handle bindGroupLayout;
};

}

} // namespace grail
} // namespace ncine

#endif
