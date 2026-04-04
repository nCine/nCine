#ifndef CLASS_GRAIL_GRAPHICSPIPELINE
#define CLASS_GRAIL_GRAPHICSPIPELINE

#include <cstdint>
#include <nctl/Array.h>
#include <nctl/String.h>
#include "grail/RenderPass.h"

namespace ncine {
namespace grail {

namespace GraphicsPipeline {

enum class VertexFormat : uint8_t
{
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,

	UINT,
	UINT2,
	UINT3,
	UINT4,

	SINT,
	SINT2,
	SINT3,
	SINT4,

	UNORM4
};

enum class VertexInputRate : uint8_t
{
	PER_VERTEX,
	PER_INSTANCE
};

struct VertexBinding
{
	uint32_t binding;
	uint32_t stride;
	VertexInputRate inputRate;
};

struct VertexAttribute
{
	uint32_t location;
	uint32_t binding;
	VertexFormat format;
	uint32_t offset;
};

struct VertexInputLayoutCreateDesc
{
	const VertexBinding *bindings = nullptr;
	uint32_t bindingCount = 0;

	const VertexAttribute *attributes = nullptr;
	uint32_t attributeCount = 0;
};

constexpr uint8_t MaxBindGroups = 4;
struct PipelineLayoutCreateDesc
{
	uint32_t bindGroupLayoutCount = 0;
	BindGroupLayout::Handle bindGroupLayouts[MaxBindGroups];
};

enum class PrimitiveTopology
{
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
	TRIANGLE_FAN
};

enum class CullMode
{
	FRONT,
	BACK,
	FRONT_AND_BACK
};

enum class CompareOp
{
	NEVER,
	LESS,
	EQUAL,
	LESS_OR_EQUAL,
	GREATER,
	NOT_EQUAL,
	GREATER_OR_EQUAL,
	ALWAYS
};

struct ShaderDesc
{
	const uint8_t *codeData = nullptr;
	size_t codeSize = 0;
	const char *entryName = "main";

	inline bool isValid() const { return (codeData && codeSize > 0 && entryName); }
};

struct CreateDesc
{
	ShaderDesc vertexShader;
	ShaderDesc fragmentShader;

	VertexInputLayoutCreateDesc vertexInputLayout;
	PipelineLayoutCreateDesc pipelineLayout;
	RenderPass::RenderTargetLayoutDesc renderTargetLayout;

	PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
	CullMode cullMode = CullMode::BACK;

	bool depthTestEnable = false;
	bool depthWriteEnable = false;
	CompareOp depthCompareOp = CompareOp::LESS;

	bool blendEnable = false;
};

struct VertexInputLayoutDesc
{
	nctl::Array<VertexBinding> bindings;
	nctl::Array<VertexAttribute> attributes;
};

struct PipelineLayoutDesc
{
	nctl::Array<BindGroupLayout::Handle> bindGroupLayouts;
};

struct Desc
{
	nctl::String vertexShaderEntry;
	nctl::String fragmentShaderEntry;

	VertexInputLayoutDesc vertexInputLayout;
	PipelineLayoutDesc pipelineLayout;
	BindGroupLayout::Handle bindGroupLayout;
	RenderPass::RenderTargetLayoutDesc renderTargetLayout;

	PrimitiveTopology primitiveTopology;
	CullMode cullMode;

	bool depthTestEnable;
	bool depthWriteEnable;
	CompareOp depthCompareOp;

	bool blendEnable;
};

}

} // namespace grail
} // namespace ncine

#endif
