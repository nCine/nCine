#include "grlapptest.h"

#include <nctl/UniquePtr.h>
#include <nctl/Optional.h>
#include <nctl/StaticArray.h>
#include <nctl/StaticString.h>

#include <ncine/Matrix4x4.h>
#include <ncine/Application.h>
#include <ncine/AppConfiguration.h>
#include <ncine/Random.h>
#include <ncine/IImageLoader.h>

#include "grail/Device.h"

#include "../../tests/apptest_datapath.h"

#ifdef WITH_EMBEDDED_SHADERS
	#include <ncine/grail/shader_arrays.h>
#else
	#include <ncine/IFile.h>
#endif

namespace grl = ncine::grail;

namespace {

nctl::Optional<grl::Device> device;

const char *TextureFile = "diffuse.png";

struct Vertex
{
	nc::Vector3f pos;
	nc::Vector3f color;
	nc::Vector2f texCoord;

	static const grl::GraphicsPipeline::VertexBinding bindings[];
	static const grl::GraphicsPipeline::VertexAttribute attributes[];
	static const grl::GraphicsPipeline::VertexInputLayoutCreateDesc layout;
};

const grl::GraphicsPipeline::VertexBinding Vertex::bindings[] = {
	{ 0, sizeof(Vertex), grl::GraphicsPipeline::VertexInputRate::PER_VERTEX }
};

const grl::GraphicsPipeline::VertexAttribute Vertex::attributes[] = {
	{ 0, 0, grl::GraphicsPipeline::VertexFormat::FLOAT3, offsetof(Vertex, pos)      },
	{ 1, 0, grl::GraphicsPipeline::VertexFormat::FLOAT3, offsetof(Vertex, color)    },
	{ 2, 0, grl::GraphicsPipeline::VertexFormat::FLOAT2, offsetof(Vertex, texCoord) }
};

const grl::GraphicsPipeline::VertexInputLayoutCreateDesc Vertex::layout = {
	bindings,
	nctl::arraySize(bindings),
	attributes,
	nctl::arraySize(attributes)
};

const uint32_t NumVertices = 4;
const Vertex vertices[NumVertices] = {
	{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.0f },  { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	{ { 0.5f, 0.5f, 0.0f },   { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
	{ { -0.5f, 0.5f, 0.0f },  { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
};

const uint32_t NumIndices = 6;
const uint16_t indices[NumIndices] = {
	0, 1, 2, 2, 3, 0
};

struct UniformBufferObject
{
	nc::Matrix4x4f model;
	nc::Matrix4x4f view;
	nc::Matrix4x4f projection;
};
float angle = 0.0f;

constexpr uint32_t NumParticles = 1024;
struct Particle
{
	nc::Vector2f position;
	nc::Vector2f velocity;
	nc::Vector4f color;
};

struct Point
{
	nc::Vector2f position;
	nc::Vector4f color;

	static const grl::GraphicsPipeline::VertexBinding bindings[];
	static const grl::GraphicsPipeline::VertexAttribute attributes[];
	static const grl::GraphicsPipeline::VertexInputLayoutCreateDesc layout;
};

const grl::GraphicsPipeline::VertexBinding Point::bindings[] = {
	{ 0, sizeof(Particle), grl::GraphicsPipeline::VertexInputRate::PER_VERTEX }
};

const grl::GraphicsPipeline::VertexAttribute Point::attributes[] = {
	{ 0, 0, grl::GraphicsPipeline::VertexFormat::FLOAT2, offsetof(Particle, position) },
	{ 1, 0, grl::GraphicsPipeline::VertexFormat::FLOAT4, offsetof(Particle, color)    }
};

const grl::GraphicsPipeline::VertexInputLayoutCreateDesc Point::layout = {
	bindings,
	nctl::arraySize(bindings),
	attributes,
	nctl::arraySize(attributes)
};

struct UboComp
{
	float deltaTime;
	float pad[3];
};

uint32_t currentBindIndex = 0;
uint32_t nextBindIndex = 0;
grl::BindGroupLayout::Handle bindGroupLayout;
grl::GraphicsPipeline::Handle gfxPipeline;
grl::BindGroupLayout::Handle bindGroupLayoutComp;
grl::ComputePipeline::Handle compPipeline;
grl::BindGroup::Handle bindGroupComp[2];
grl::Buffer::Handle storageBuffer[2];
uint64_t lastBufferWrite[2] = { 0, 0 };
grl::GraphicsPipeline::Handle pointPipeline;

grl::Texture::Handle depthTexture;
grl::TextureView::Handle depthTextureView;
grl::Buffer::Handle vertexBuffer;
grl::Buffer::Handle indexBuffer;
grl::Texture::Handle texture;
grl::TextureView::Handle textureView;
grl::BindGroup::Handle bindGroup;

grl::Format depthFormat = grl::Format::UNDEFINED;

#ifndef WITH_EMBEDDED_SHADERS
nctl::UniquePtr<uint8_t[]> loadShader(grl::GraphicsPipeline::ShaderDesc &shaderDesc, const char *filename)
{
	nctl::UniquePtr<uint8_t[]> codeBuffer;

	nctl::UniquePtr<nc::IFile> shaderFile = nc::IFile::createFileHandle(filename);
	shaderFile->open(nc::IFile::OpenMode::READ | nc::IFile::OpenMode::BINARY);
	if (shaderFile->isOpened() == true)
	{
		shaderDesc.codeSize = shaderFile->size();
		codeBuffer = nctl::makeUnique<uint8_t[]>(shaderDesc.codeSize);
		shaderFile->read(codeBuffer.get(), shaderDesc.codeSize);
		shaderFile->close();
		shaderDesc.codeData = codeBuffer.get();
	}

	return codeBuffer;
}
#endif

void initParticles(Particle *particles, uint32_t count)
{
	ASSERT(particles != nullptr);
	ASSERT(count > 0);
	nc::Random &r = nc::random();

	for (uint32_t i = 0; i < count; i++)
	{
		Particle &p = particles[i];
		p.position.set(r.fastReal(-1.0f, 1.0f), r.fastReal(-1.0f, 1.0f));
		p.velocity.set(r.fastReal(-1.0f, 1.0f), r.fastReal(-1.0f, 1.0f));
		p.color.set(r.fastReal(0.0f, 1.0f), r.fastReal(0.0f, 1.0f), r.fastReal(0.0f, 1.0f), 1.0f);
	}
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	config.withScenegraph = false;
	config.withAudio = false;
	config.withJobSystem = false;
	config.resizable = false;

	setDataPath(config);
}

void MyEventHandler::onInit()
{
	grl::Device::CreateDesc deviceDesc;
	device.emplace(grl::Device::create(deviceDesc));

	grl::Swapchain::Desc swapchainDesc;
	swapchainDesc.format = grl::Format::B8G8R8A8_SRGB;
	swapchainDesc.width = nc::theApplication().width();
	swapchainDesc.height = nc::theApplication().height();
	device->createSwapchain(swapchainDesc);
	const grl::Swapchain &swapchain = device->swapchain();

	depthFormat = device->findDepthFormat(grl::Device::DepthFormatQuery());

// -------------------------------------
{
	grl::BindGroupLayout::Entry bindGroupLayoutEntries[3];
	bindGroupLayoutEntries[0].binding = 0;
	bindGroupLayoutEntries[0].type = grl::BindGroupLayout::BindingType::UNIFORM_BUFFER;
	bindGroupLayoutEntries[0].visibility = grl::BindGroupLayout::ShaderStage::COMPUTE;
	bindGroupLayoutEntries[0].dynamicOffset = true;
	bindGroupLayoutEntries[1].binding = 1;
	bindGroupLayoutEntries[1].type = grl::BindGroupLayout::BindingType::STORAGE_BUFFER;
	bindGroupLayoutEntries[1].visibility = grl::BindGroupLayout::ShaderStage::COMPUTE;
	bindGroupLayoutEntries[2].binding = 2;
	bindGroupLayoutEntries[2].type = grl::BindGroupLayout::BindingType::STORAGE_BUFFER;
	bindGroupLayoutEntries[2].visibility = grl::BindGroupLayout::ShaderStage::COMPUTE;
	grl::BindGroupLayout::CreateDesc bindGroupLayoutDesc;
	bindGroupLayoutDesc.entries = bindGroupLayoutEntries;
	bindGroupLayoutDesc.entryCount = nctl::arraySize(bindGroupLayoutEntries);
	bindGroupLayoutDesc.debugName = "Bind group layout compute";

	bindGroupLayoutComp = device->createBindGroupLayout(bindGroupLayoutDesc);
	ASSERT(bindGroupLayoutComp.isValid());

	grl::ComputePipeline::CreateDesc computePipelineDesc;
#ifdef WITH_EMBEDDED_SHADERS
	computePipelineDesc.computeShader.codeData = reinterpret_cast<const uint8_t *>(grl::ShaderArrays::particles_comp);
	computePipelineDesc.computeShader.codeSize = grl::ShaderArrays::particles_comp_size * sizeof(uint32_t);
#else
	const nctl::String shaderPath = nc::fs::executableDir() + "/../../shaders/particles.comp.spv";
	nctl::UniquePtr<uint8_t[]> shaderBuffer = loadShader(computePipelineDesc.computeShader, shaderPath.data());
#endif
	computePipelineDesc.computeShader.debugName = "particles.comp";

	computePipelineDesc.pipelineLayout.bindGroupLayoutCount = 1;
	computePipelineDesc.pipelineLayout.bindGroupLayouts[0] = bindGroupLayoutComp;
	computePipelineDesc.debugName = "Particles";

	compPipeline = device->createComputePipeline(computePipelineDesc);
	ASSERT(compPipeline.isValid());

	Particle particles[NumParticles];
	initParticles(particles, NumParticles);

	grl::Buffer::Desc stagingStorageBufferDesc;
	stagingStorageBufferDesc.size = NumParticles * sizeof(Particle);
	stagingStorageBufferDesc.usage = grl::Buffer::Usage::TRANSFER_SRC;
	stagingStorageBufferDesc.memoryType = grl::Buffer::MemoryType::UPLOAD;
	grl::Buffer::Handle stagingStorageBuffer = device->createBuffer(stagingStorageBufferDesc);

	void *particlesData = device->mapBuffer(stagingStorageBuffer);
	FATAL_ASSERT(particlesData != nullptr);
	memcpy(particlesData, particles, device->bufferDesc(stagingStorageBuffer).size);
	device->unmapBuffer(stagingStorageBuffer);

	grl::Buffer::Desc storageBufferDesc;
	storageBufferDesc.size = NumParticles * sizeof(Particle);
	storageBufferDesc.usage = grl::Buffer::Usage::VERTEX | grl::Buffer::Usage::STORAGE | grl::Buffer::Usage::TRANSFER_DST;
	storageBufferDesc.memoryType = grl::Buffer::MemoryType::DEFAULT;
	storageBufferDesc.debugName = "Particles buffer 0";
	storageBuffer[0] = device->createBuffer(storageBufferDesc);
	storageBufferDesc.debugName = "Particles buffer 1";
	storageBuffer[1] = device->createBuffer(storageBufferDesc);

	grl::CommandBuffer cmdCopy = device->createCommandBuffer();
	cmdCopy.begin();
	cmdCopy.copyBuffer(stagingStorageBuffer, storageBuffer[0]);
	cmdCopy.end();

	const uint64_t copySubmissionId = device->submitCommandBuffer(cmdCopy);
	device->waitSubmission(copySubmissionId);

	device->destroyBuffer(stagingStorageBuffer);

	UboComp uboComp;
	uboComp.deltaTime = nc::theApplication().frameTime();
	const grl::Buffer::DynamicAlloc dynAllocComp = device->allocateDynamicBuffer(sizeof(UboComp));
	FATAL_ASSERT_MSG(dynAllocComp.isValid(), "Dynamic buffer allocation has failed");
	memcpy(dynAllocComp.mapPtr, &uboComp, sizeof(UboComp));

	for (uint32_t i = 0; i < 2; i++)
	{
		nctl::StaticString<64> debugNameString;
		if (bindGroupComp[i].isValid() == false)
		{
			grl::BindGroup::Desc bindGroupDesc;
			bindGroupDesc.layout = bindGroupLayoutComp;
			grl::BindGroup::Entry bindGroupEntries[3];
			bindGroupEntries[0].binding = 0;
			bindGroupEntries[0].buffer = dynAllocComp.handle;
			bindGroupEntries[0].size = sizeof(UboComp);
			bindGroupEntries[1].binding = 1;
			bindGroupEntries[1].buffer = storageBuffer[i];
			bindGroupEntries[1].size = NumParticles * sizeof(Particle);
			bindGroupEntries[2].binding = 2;
			bindGroupEntries[2].buffer = storageBuffer[(i + 1) % 2];
			bindGroupEntries[2].size = NumParticles * sizeof(Particle);
			bindGroupDesc.entries = bindGroupEntries;
			bindGroupDesc.entryCount = nctl::arraySize(bindGroupEntries);

			debugNameString.format("Particle simulation bindgroup #%u", i);
			bindGroupDesc.debugName = debugNameString.data();
			bindGroupComp[i] = device->createBindGroup(bindGroupDesc);
		}
	}

	grl::GraphicsPipeline::CreateDesc pointPipelineDesc;
#ifdef WITH_EMBEDDED_SHADERS
	pointPipelineDesc.vertexShader.codeData = reinterpret_cast<const uint8_t *>(grl::ShaderArrays::point_particles_vert);
	pointPipelineDesc.vertexShader.codeSize = grl::ShaderArrays::point_particles_vert_size * sizeof(uint32_t);
	pointPipelineDesc.fragmentShader.codeData = reinterpret_cast<const uint8_t *>(grl::ShaderArrays::point_particles_frag);
	pointPipelineDesc.fragmentShader.codeSize = grl::ShaderArrays::point_particles_frag_size * sizeof(uint32_t);
#else
	const nctl::String shaderPathVert = nc::fs::executableDir() + "/../../shaders/point_particles.vert.spv";
	nctl::UniquePtr<uint8_t[]> shaderBufferVert = loadShader(pointPipelineDesc.vertexShader, shaderPathVert.data());
	const nctl::String shaderPathFrag = nc::fs::executableDir() + "/../../shaders/point_particles.frag.spv";
	nctl::UniquePtr<uint8_t[]> shaderBufferFrag = loadShader(pointPipelineDesc.fragmentShader, shaderPathFrag.data());
#endif
	pointPipelineDesc.vertexShader.debugName = "point_particles.vert";
	pointPipelineDesc.fragmentShader.debugName = "point_particles.frag";

	pointPipelineDesc.vertexInputLayout = Point::layout;
	pointPipelineDesc.renderTargetLayout.colorCount = 1;
	pointPipelineDesc.renderTargetLayout.colors[0].format = swapchainDesc.format;
	pointPipelineDesc.renderTargetLayout.depth.format = depthFormat;
	pointPipelineDesc.primitiveTopology = grl::GraphicsPipeline::PrimitiveTopology::POINT_LIST;
	pointPipelineDesc.depthWriteEnable = false;

	grl::GraphicsPipeline::ColorBlendAttachmentDesc blendAttachment{};
	blendAttachment.blendEnable = true;
	blendAttachment.srcColorFactor = grl::GraphicsPipeline::BlendFactor::SRC_ALPHA;
	blendAttachment.dstColorFactor = grl::GraphicsPipeline::BlendFactor::ONE_MINUS_SRC_ALPHA;
	blendAttachment.srcAlphaFactor = grl::GraphicsPipeline::BlendFactor::ONE_MINUS_SRC_ALPHA;
	blendAttachment.dstAlphaFactor = grl::GraphicsPipeline::BlendFactor::ZERO;
	pointPipelineDesc.blendState.attachments = &blendAttachment;
	pointPipelineDesc.blendState.attachmentCount = 1;
	pointPipelineDesc.debugName = "Point particles";

	pointPipeline = device->createGraphicsPipeline(pointPipelineDesc);
	ASSERT(pointPipeline.isValid());
}
// -------------------------------------

	grl::BindGroupLayout::Entry bindGroupLayoutEntries[2];
	bindGroupLayoutEntries[0].binding = 0;
	bindGroupLayoutEntries[0].type = grl::BindGroupLayout::BindingType::UNIFORM_BUFFER;
	bindGroupLayoutEntries[0].visibility = grl::BindGroupLayout::ShaderStage::VERTEX;
	bindGroupLayoutEntries[0].dynamicOffset = true;
	bindGroupLayoutEntries[1].binding = 1;
	bindGroupLayoutEntries[1].type = grl::BindGroupLayout::BindingType::COMBINED_TEXTURE_SAMPLER;
	bindGroupLayoutEntries[1].visibility = grl::BindGroupLayout::ShaderStage::FRAGMENT | grl::BindGroupLayout::ShaderStage::VERTEX;
	grl::BindGroupLayout::CreateDesc bindGroupLayoutDesc;
	bindGroupLayoutDesc.entries = bindGroupLayoutEntries;
	bindGroupLayoutDesc.entryCount = nctl::arraySize(bindGroupLayoutEntries);
	bindGroupLayoutDesc.debugName = "Bind group layout textured";

	bindGroupLayout = device->createBindGroupLayout(bindGroupLayoutDesc);
	ASSERT(bindGroupLayout.isValid());

	grl::GraphicsPipeline::CreateDesc gfxPipelineDesc;
#ifdef WITH_EMBEDDED_SHADERS
	gfxPipelineDesc.vertexShader.codeData = reinterpret_cast<const uint8_t *>(grl::ShaderArrays::texture_vert);
	gfxPipelineDesc.vertexShader.codeSize = grl::ShaderArrays::texture_vert_size * sizeof(uint32_t);
	gfxPipelineDesc.fragmentShader.codeData = reinterpret_cast<const uint8_t *>(grl::ShaderArrays::texture_frag);
	gfxPipelineDesc.fragmentShader.codeSize = grl::ShaderArrays::texture_frag_size * sizeof(uint32_t);
#else
	const nctl::String shaderPathVertex = nc::fs::executableDir() + "/../../shaders/texture.vert.spv";
	nctl::UniquePtr<uint8_t[]> shaderBufferVertex = loadShader(gfxPipelineDesc.vertexShader, shaderPathVertex.data());
	const nctl::String shaderPathFragment = nc::fs::executableDir() + "/../../shaders/texture.frag.spv";
	nctl::UniquePtr<uint8_t[]> shaderBufferFragment = loadShader(gfxPipelineDesc.fragmentShader, shaderPathFragment.data());
#endif
	gfxPipelineDesc.vertexShader.debugName = "texture.vert";
	gfxPipelineDesc.fragmentShader.debugName = "texture.frag";

	gfxPipelineDesc.vertexInputLayout = Vertex::layout;
	gfxPipelineDesc.pipelineLayout.bindGroupLayoutCount = 1;
	gfxPipelineDesc.pipelineLayout.bindGroupLayouts[0] = bindGroupLayout;
	gfxPipelineDesc.renderTargetLayout.colorCount = 1;
	gfxPipelineDesc.renderTargetLayout.colors[0].format = swapchainDesc.format;
	gfxPipelineDesc.renderTargetLayout.depth.format = depthFormat;
	gfxPipelineDesc.debugName = "Textured quad";

	gfxPipeline = device->createGraphicsPipeline(gfxPipelineDesc);
	ASSERT(gfxPipeline.isValid());

	grl::Texture::Desc depthTextureDesc;
	depthTextureDesc.width = swapchain.desc().width;
	depthTextureDesc.height = swapchain.desc().height;
	depthTextureDesc.format = depthFormat;
	depthTextureDesc.usage = grl::Texture::Usage::DEPTH_STENCIL;
	depthTextureDesc.debugName = "Depth texture";
	depthTexture = device->createTexture(depthTextureDesc);
	grl::TextureView::Desc depthTextureviewDesc;
	depthTextureviewDesc.texture = depthTexture;
	depthTextureviewDesc.format = depthTextureDesc.format;
	depthTextureviewDesc.debugName = "Depth texture view";
	depthTextureView = device->createTextureView(depthTextureviewDesc);

	grl::Buffer::Desc stagingVertexBufferDesc;
	stagingVertexBufferDesc.size = sizeof(Vertex) * NumVertices;
	stagingVertexBufferDesc.usage = grl::Buffer::Usage::TRANSFER_SRC;
	stagingVertexBufferDesc.memoryType = grl::Buffer::MemoryType::UPLOAD;
	grl::Buffer::Handle stagingVertexBuffer = device->createBuffer(stagingVertexBufferDesc);

	grl::Buffer::Desc vertexBufferDesc;
	vertexBufferDesc.size = sizeof(Vertex) * NumVertices;
	vertexBufferDesc.usage = grl::Buffer::Usage::VERTEX | grl::Buffer::Usage::TRANSFER_DST;
	vertexBufferDesc.memoryType = grl::Buffer::MemoryType::DEFAULT;
	vertexBufferDesc.debugName = "Vertex buffer";
	vertexBuffer = device->createBuffer(vertexBufferDesc);

	void *vertexData = device->mapBuffer(stagingVertexBuffer);
	FATAL_ASSERT(vertexData != nullptr);
	memcpy(vertexData, vertices, device->bufferDesc(stagingVertexBuffer).size);
	device->unmapBuffer(stagingVertexBuffer);

	grl::Buffer::Desc stagingIndexBufferDesc;
	stagingIndexBufferDesc.size = sizeof(uint16_t) * NumIndices;
	stagingIndexBufferDesc.usage = grl::Buffer::Usage::TRANSFER_SRC;
	stagingIndexBufferDesc.memoryType = grl::Buffer::MemoryType::UPLOAD;
	grl::Buffer::Handle stagingIndexBuffer = device->createBuffer(stagingIndexBufferDesc);

	grl::Buffer::Desc indexBufferDesc;
	indexBufferDesc.size = sizeof(uint16_t) * NumIndices;
	indexBufferDesc.usage = grl::Buffer::Usage::INDEX | grl::Buffer::Usage::TRANSFER_DST;
	indexBufferDesc.memoryType = grl::Buffer::MemoryType::DEFAULT;
	indexBufferDesc.debugName = "Index buffer";
	indexBuffer = device->createBuffer(indexBufferDesc);

	void *indexData = device->mapBuffer(stagingIndexBuffer);
	FATAL_ASSERT(indexData != nullptr);
	memcpy(indexData, indices, device->bufferDesc(stagingIndexBuffer).size);
	device->unmapBuffer(stagingIndexBuffer);

	nctl::UniquePtr<nc::IImageLoader> imgLoader = nc::IImageLoader::createFromFile((prefixDataPath("textures", TextureFile)).data());
	grl::Buffer::Desc stagingTextureBufferDesc;
	stagingTextureBufferDesc.size = imgLoader->dataSize();
	stagingTextureBufferDesc.usage = grl::Buffer::Usage::TRANSFER_SRC;
	stagingTextureBufferDesc.memoryType = grl::Buffer::MemoryType::UPLOAD;
	grl::Buffer::Handle stagingTextureBuffer = device->createBuffer(stagingTextureBufferDesc);

	grl::Texture::Desc textureDesc;
	textureDesc.width = imgLoader->width();
	textureDesc.height = imgLoader->height();
	textureDesc.format = grl::Format::R8G8B8A8_SRGB;
	if (imgLoader->format() == nc::IImageLoader::Format::RGB8)
		textureDesc.format = grl::Format::R8G8B8_SRGB;
	textureDesc.usage = grl::Texture::Usage::SAMPLED | grl::Texture::Usage::TRANSFER_DST;
	textureDesc.debugName = TextureFile;
	texture = device->createTexture(textureDesc);

	grl::TextureView::Desc textureViewDesc;
	textureViewDesc.texture = texture;
	textureViewDesc.format = textureDesc.format;
	textureViewDesc.sampler.magFilter = grl::TextureView::Filter::LINEAR;
	textureViewDesc.sampler.minFilter = grl::TextureView::Filter::LINEAR;
	textureViewDesc.debugName = "Texture view";
	textureView = device->createTextureView(textureViewDesc);

	void *textureData = device->mapBuffer(stagingTextureBuffer);
	FATAL_ASSERT(textureData != nullptr);
	memcpy(textureData, imgLoader->pixels(), device->bufferDesc(stagingTextureBuffer).size);
	device->unmapBuffer(stagingTextureBuffer);

	grl::CommandBuffer cmdCopy = device->createCommandBuffer("Copy vertex and index buffers");
	cmdCopy.begin();
	cmdCopy.copyBuffer(stagingVertexBuffer, vertexBuffer);
	cmdCopy.copyBuffer(stagingIndexBuffer, indexBuffer);
	cmdCopy.transitionTexture(texture, grl::Texture::State::TRANSFER_DST);
	cmdCopy.copyBufferToTexture(stagingTextureBuffer, texture);
	cmdCopy.transitionTexture(texture, grl::Texture::State::SHADER_READ);
	cmdCopy.end();

	const uint64_t copySubmissionId = device->submitCommandBuffer(cmdCopy);
	device->waitSubmission(copySubmissionId);

	device->destroyBuffer(stagingVertexBuffer);
	device->destroyBuffer(stagingIndexBuffer);
	device->destroyBuffer(stagingTextureBuffer);
}

void MyEventHandler::onFrameStart()
{
	nextBindIndex = (currentBindIndex + 1) % 2;

	grl::Swapchain &swapchain = device->swapchain();
	grl::Extent2D swapchainExtent{ swapchain.desc().width, swapchain.desc().height };

	const grl::Swapchain::AcquireDesc acquireDesc = swapchain.acquireNextTextureView();
	if (acquireDesc.result == grl::Swapchain::AcquireResult::OUT_OF_DATE)
		swapchain.recreate();
	FATAL_ASSERT_MSG(acquireDesc.result == grl::Swapchain::AcquireResult::SUCCESS ||
	                 acquireDesc.result == grl::Swapchain::AcquireResult::SUBOPTIMAL,
	                 "Failed to acquire swap chain image");

	const float frameTime = nc::theApplication().frameTime();
	angle += 10.0f * frameTime;
	if (angle > 360.0f)
		angle -= 360.0f;
	UniformBufferObject ubo;
	ubo.model.rotateZ(angle);
	const float aspect = swapchainExtent.width / static_cast<float>(swapchainExtent.height);
	ubo.projection = nc::Matrix4x4f::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

	const grl::Buffer::DynamicAlloc dynAlloc = device->allocateDynamicBuffer(sizeof(ubo));
	FATAL_ASSERT_MSG(dynAlloc.isValid(), "Dynamic buffer allocation has failed");
	memcpy(dynAlloc.mapPtr, &ubo, sizeof(ubo));

	if (bindGroup.isValid() == false)
	{
		grl::BindGroup::Desc bindGroupDesc;
		bindGroupDesc.layout = bindGroupLayout;
		grl::BindGroup::Entry bindGroupEntries[2];
		bindGroupEntries[0].binding = 0;
		bindGroupEntries[0].buffer = dynAlloc.handle;
		bindGroupEntries[0].size = sizeof(ubo);
		bindGroupEntries[1].binding = 1;
		bindGroupEntries[1].textureView = textureView;
		bindGroupDesc.entries = bindGroupEntries;
		bindGroupDesc.entryCount = nctl::arraySize(bindGroupEntries);
		bindGroupDesc.debugName = "Texture bind group";
		bindGroup = device->createBindGroup(bindGroupDesc);
	}

	grl::RenderPass::RenderPassDesc renderPassDesc;
	renderPassDesc.colorLoadOps[0] = grl::RenderPass::LoadOp::CLEAR;
	renderPassDesc.depthLoadOp = grl::RenderPass::LoadOp::CLEAR;
	renderPassDesc.layout.colorCount = 1;
	renderPassDesc.layout.colors[0].format = swapchain.desc().format;
	renderPassDesc.layout.depth.format = depthFormat;
	grl::RenderPass::RenderTargetDesc renderTargetDesc;
	renderTargetDesc.color[0] = acquireDesc.textureView;
	renderTargetDesc.depth = depthTextureView;
	renderTargetDesc.width = swapchainExtent.width;
	renderTargetDesc.height = swapchainExtent.height;

	UboComp uboComp;
	uboComp.deltaTime = nc::theApplication().frameTime();
	const grl::Buffer::DynamicAlloc dynAllocComp = device->allocateDynamicBuffer(sizeof(uboComp));
	FATAL_ASSERT_MSG(dynAllocComp.isValid(), "Dynamic buffer allocation has failed");
	memcpy(dynAllocComp.mapPtr, &uboComp, sizeof(UboComp));

	grl::CommandBuffer cmd = device->createCommandBuffer("Draw textured quad and particles");
	cmd.begin();
	cmd.transitionBuffer(storageBuffer[currentBindIndex], grl::Buffer::State::VERTEX_BUFFER);
	cmd.beginRenderPass(renderPassDesc, renderTargetDesc, "Graphics render pass");
	cmd.beginLabel("Rendering quad");
	cmd.bindGraphicsPipeline(gfxPipeline);
	cmd.bindVertexBuffer(vertexBuffer);
	cmd.bindIndexBuffer(indexBuffer);
	cmd.bindBindGroup(bindGroup, &dynAlloc.offset, 1);
	cmd.setViewport(grl::Offset2D{ 0, 0 }, swapchainExtent, 0.0f, 1.0f);
	cmd.setScissor(grl::Offset2D{ 0, 0 }, swapchainExtent);
	cmd.drawIndexed(NumIndices, 1, 0, 0, 0);
	cmd.endLabel();
	cmd.beginLabel("Rendering particles");
	cmd.bindGraphicsPipeline(pointPipeline);
	cmd.bindVertexBuffer(storageBuffer[currentBindIndex]);
	cmd.draw(NumParticles, 1, 0, 0);
	cmd.endLabel();
	cmd.endRenderPass();
	cmd.beginLabel("Simulate particles");
	cmd.bindComputePipeline(compPipeline);
	cmd.bindBindGroup(bindGroupComp[currentBindIndex], &dynAllocComp.offset, 1);
	cmd.transitionBuffer(storageBuffer[nextBindIndex], grl::Buffer::State::STORAGE_WRITE);
	cmd.dispatch(NumParticles / 256, 1, 1);
	cmd.endLabel();
	cmd.end();

	grl::Device::SubmitDesc submitDesc{ true, true, lastBufferWrite[currentBindIndex] };
	lastBufferWrite[nextBindIndex] = device->submitCommandBuffer(cmd, submitDesc);
	device->swapchain().present();

	currentBindIndex = nextBindIndex;
}

void MyEventHandler::onResizeWindow(int width, int height)
{
	device->swapchain().resize(width, height);
}

void MyEventHandler::onShutdown()
{
	device->waitIdle();

	device->destroyBindGroup(bindGroupComp[1]);
	device->destroyBindGroup(bindGroupComp[0]);
	device->destroyBuffer(storageBuffer[1]);
	device->destroyBuffer(storageBuffer[0]);
	device->destroyComputePipeline(compPipeline);
	device->destroyBindGroupLayout(bindGroupLayoutComp);

	device->destroyGraphicsPipeline(pointPipeline);

	device->destroyBindGroup(bindGroup);
	device->destroyTextureView(textureView);
	device->destroyTexture(texture);
	device->destroyBuffer(indexBuffer);
	device->destroyBuffer(vertexBuffer);
	device->destroyTextureView(depthTextureView);
	device->destroyTexture(depthTexture);
	device->destroyGraphicsPipeline(gfxPipeline);
	device->destroyBindGroupLayout(bindGroupLayout);

	device->destroy();
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
