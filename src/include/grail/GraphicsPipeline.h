#ifndef CLASS_GRAIL_GRAPHICSPIPELINE
#define CLASS_GRAIL_GRAPHICSPIPELINE

#include <nctl/String.h>
#include "grail/pipeline.h"
#include "grail/RenderPass.h"

namespace ncine {
namespace grail {

namespace GraphicsPipeline {

using pipeline::MaxBindGroups;
using pipeline::PipelineLayoutCreateDesc;
using pipeline::ShaderDesc;
using pipeline::PipelineLayoutDesc;

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

enum class PrimitiveTopology
{
	POINT_LIST,
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

enum class BlendFactor
{
	ZERO,
	ONE,

	SRC_COLOR,
	ONE_MINUS_SRC_COLOR,

	DST_COLOR,
	ONE_MINUS_DST_COLOR,

	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,

	DST_ALPHA,
	ONE_MINUS_DST_ALPHA,

	CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR,

	CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA,

	SRC_ALPHA_SATURATE
};

enum class BlendOp
{
	ADD,
	SUBTRACT,
	REVERSE_SUBTRACT,
	MIN,
	MAX
};

enum class ColorComponent : uint8_t
{
	NONE = 0,
	R = 1 << 0,
	G = 1 << 1,
	B = 1 << 2,
	A = 1 << 3,
	ALL = R | G | B | A
};

struct ColorBlendAttachmentDesc
{
	bool blendEnable = false;

	BlendFactor srcColorFactor = BlendFactor::ONE;
	BlendFactor dstColorFactor = BlendFactor::ZERO;
	BlendOp colorOp = BlendOp::ADD;

	BlendFactor srcAlphaFactor = BlendFactor::ONE;
	BlendFactor dstAlphaFactor = BlendFactor::ZERO;
	BlendOp alphaOp = BlendOp::ADD;

	ColorComponent colorWriteMask = ColorComponent::ALL;
};

struct ColorBlendStateCreateDesc
{
	const ColorBlendAttachmentDesc *attachments = nullptr;
	uint32_t attachmentCount = 0;

	float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
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

	ColorBlendStateCreateDesc blendState;

	const char *debugName = nullptr;
};

struct VertexInputLayoutDesc
{
	nctl::Array<VertexBinding> bindings;
	nctl::Array<VertexAttribute> attributes;
};

struct ColorBlendStateDesc
{
	nctl::Array<ColorBlendAttachmentDesc> attachments;

	float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
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

	ColorBlendStateDesc blendState;
};

}

} // namespace grail
} // namespace ncine

#endif
