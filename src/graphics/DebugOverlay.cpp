#include "DebugOverlay.h"
#include "Application.h"
#include "StackedBarPlotter.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h"

#include "RenderStatistics.h"
#ifdef WITH_LUA
	#include "LuaStatistics.h"
#endif

#include "version.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

DebugOverlay::DebugOverlay(const AppConfiguration &appCfg)
    : IDebugOverlay(appCfg), plotterInitialized_(false), infoTextInitialized_(false),
      infoStringTopLeft_(MaxTextLength), infoStringTopRight_(MaxTextLength),
      infoStringBottomRight_(MaxTextLength)
{
	if (appCfg.withProfilerGraphs)
		initPlotter();
	if (appCfg.withInfoText)
		initInfoText();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void DebugOverlay::update()
{
	if (plotterInitialized_)
		profilePlotter_->setEnabled(settings_.showProfilerGraphs);

	if (infoTextInitialized_)
	{
		infoLineTopLeft_->setEnabled(settings_.showInfoText);
		infoLineTopRight_->setEnabled(settings_.showInfoText);
		infoLineBottomLeft_->setEnabled(settings_.showInfoText);
		infoLineBottomRight_->setEnabled(settings_.showInfoText);
	}

	if (infoTextInitialized_ && updateTimer_.interval() > updateTime_)
	{
		infoStringTopLeft_.clear();
		appendMoreRenderCommandsStatistics(infoStringTopLeft_);
		infoLineTopLeft_->setString(infoStringTopLeft_);
		infoLineTopLeft_->setPosition(infoLineTopLeft_->width() * 0.5f, theApplication().height() - infoLineTopLeft_->height() * 0.5f);

		infoStringTopRight_.format("FPS: %.0f (%.2fms)\n", 1.0f / theApplication().interval(), theApplication().interval() * 1000.0f);
		appendRenderCommandsStatistics(infoStringTopRight_);
		infoLineTopRight_->setString(infoStringTopRight_);
		infoLineTopRight_->setPosition(theApplication().width() - infoLineTopRight_->width() * 0.5f, theApplication().height() - infoLineTopRight_->height() * 0.5f);

#ifdef WITH_LUA
		infoStringBottomRight_.clear();
		appendLuaStatistics(infoStringBottomRight_);
		infoLineBottomRight_->setString(infoStringBottomRight_);
		infoLineBottomRight_->setPosition(theApplication().width() - infoLineBottomRight_->width() * 0.5f, infoLineBottomRight_->height() * 0.5f + infoLineBottomRight_->fontBase() * 0.25f);
#endif
		updateTimer_.start();
	}
}

void DebugOverlay::updateFrameTimings()
{
	if (profilePlotter_)
	{
		const float *timings = theApplication().timings();

		profilePlotter_->addValue(0, timings[Application::Timings::UPDATE]);
		profilePlotter_->addValue(1, timings[Application::Timings::VISIT]);
		profilePlotter_->addValue(2, timings[Application::Timings::DRAW]);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void DebugOverlay::initPlotter()
{
	const Rectf rect(theApplication().width() * 0.1f, theApplication().height() * 0.1f,
	                 theApplication().width() * 0.8f, theApplication().height() * 0.15f);
	profilePlotter_ = nctl::makeUnique<StackedBarPlotter>(&theApplication().rootNode(), rect);
	profilePlotter_->setBackgroundColor(Color(89, 89, 115, 128));
	profilePlotter_->addVariable(50, 0.2f);
	profilePlotter_->variable(0).setGraphColor(Color(204, 0, 0));
	profilePlotter_->variable(0).setMeanColor(Color(255, 0, 0));
	profilePlotter_->addVariable(50, 0.2f);
	profilePlotter_->variable(1).setGraphColor(Color(0, 204, 0));
	profilePlotter_->variable(1).setMeanColor(Color(0, 255, 0));
	profilePlotter_->addVariable(50, 0.2f);
	profilePlotter_->variable(2).setGraphColor(Color(0, 0, 204));
	profilePlotter_->variable(2).setMeanColor(Color(0, 0, 255));

	profilePlotter_->variable(0).setPlotMean(false);
	profilePlotter_->variable(1).setPlotMean(false);
	profilePlotter_->variable(2).setPlotMean(false);
	profilePlotter_->setPlotRefValue(true);
	profilePlotter_->setRefValue(1.0f / 60.0f); // 60 FPS

	plotterInitialized_ = true;
}

void DebugOverlay::initInfoText()
{
	SceneNode *rootNode = &theApplication().rootNode();
	const AppConfiguration &appCfg = theApplication().appConfiguration();

	nctl::String fontTexFilePath = IFile::dataPath() + appCfg.fontTexFilename;
	nctl::String fontFntFilePath = IFile::dataPath() + appCfg.fontFntFilename;
	if (IFile::access(fontTexFilePath.data(), IFile::AccessMode::EXISTS) == false)
		LOGW_X("Cannot access font texture file \"%s\" to enable information text", fontTexFilePath.data());
	else if (IFile::access(fontFntFilePath.data(), IFile::AccessMode::EXISTS) == false)
		LOGW_X("Cannot access font FNT file \"%s\" to enable information text", fontFntFilePath.data());
	else
	{
		font_ = nctl::makeUnique<Font>(fontTexFilePath.data(), fontFntFilePath.data());

		infoLineTopLeft_ = nctl::makeUnique<TextNode>(rootNode, font_.get());
		infoLineTopLeft_->setLayer(DrawableNode::LayerBase::HUD + 3);
		infoLineTopLeft_->setAlignment(TextNode::Alignment::LEFT);
		infoLineTopLeft_->setScale(0.75f);

		infoLineTopRight_ = nctl::makeUnique<TextNode>(rootNode, font_.get());
		infoLineTopRight_->setLayer(DrawableNode::LayerBase::HUD + 3);
		infoLineTopRight_->setAlignment(TextNode::Alignment::RIGHT);
		infoLineTopRight_->setScale(0.75f);

		infoLineBottomLeft_ = nctl::makeUnique<TextNode>(rootNode, font_.get());
		infoLineBottomLeft_->setLayer(DrawableNode::LayerBase::HUD + 3);
		infoLineBottomLeft_->setAlignment(TextNode::Alignment::LEFT);
		infoLineBottomLeft_->setScale(0.75f);

		infoLineBottomRight_ = nctl::makeUnique<TextNode>(rootNode, font_.get());
		infoLineBottomRight_->setLayer(DrawableNode::LayerBase::HUD + 3);
		infoLineBottomRight_->setAlignment(TextNode::Alignment::RIGHT);
		infoLineBottomRight_->setScale(0.75f);

		nctl::String infoStringBottomLeft(MaxTextLength);
#ifdef WITH_GIT_VERSION
		infoStringBottomLeft.format("%s (%s)", VersionStrings::Version, VersionStrings::GitBranch);
#else
		infoStringBottomLeft.format("%s at %s", VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#endif
		infoLineBottomLeft_->setString(infoStringBottomLeft);
		infoLineBottomLeft_->setPosition(infoLineBottomLeft_->width() * 0.5f, infoLineBottomLeft_->height() * 0.5f + infoLineBottomLeft_->fontBase() * 0.25f);
	}

	infoTextInitialized_ = true;
}

void DebugOverlay::appendRenderCommandsStatistics(nctl::String &string)
{
	const RenderStatistics::Commands &spriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::SPRITE);
	const RenderStatistics::Commands &meshspriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::MESH_SPRITE);
	const RenderStatistics::Commands &particleCommands = RenderStatistics::commands(RenderCommand::CommandTypes::PARTICLE);
	const RenderStatistics::Commands &textCommands = RenderStatistics::commands(RenderCommand::CommandTypes::TEXT);
	const RenderStatistics::Commands &plotterCommands = RenderStatistics::commands(RenderCommand::CommandTypes::PLOTTER);
#ifdef WITH_IMGUI
	const RenderStatistics::Commands &imguiCommands = RenderStatistics::commands(RenderCommand::CommandTypes::IMGUI);
#endif
	const RenderStatistics::Commands &allCommands = RenderStatistics::allCommands();

	string.formatAppend(
	    "Sprites: %uV, %uDC (%u Tr), %uI/%uB\n"
	    "Mesh Sprites: %uV, %uDC (%u Tr), %uI/%uB\n"
	    "Particles: %uV, %uDC (%u Tr), %uI/%uB\n"
	    "Text: %uV, %uDC (%u Tr), %uI/%uB\n"
	    "Plotter: %uV, %uDC (%u Tr), %uI/%uB\n"
#ifdef WITH_IMGUI
	    "ImGui: %uV, %uDC (%u Tr), %uI/%uB\n"
#endif
	    "Total: %uV, %uDC (%u Tr), %uI/%uB\n",
	    spriteCommands.vertices, spriteCommands.commands, spriteCommands.transparents, spriteCommands.instances, spriteCommands.batchSize,
	    meshspriteCommands.vertices, meshspriteCommands.commands, meshspriteCommands.transparents, meshspriteCommands.instances, meshspriteCommands.batchSize,
	    particleCommands.vertices, particleCommands.commands, particleCommands.transparents, particleCommands.instances, particleCommands.batchSize,
	    textCommands.vertices, textCommands.commands, textCommands.transparents, textCommands.instances, textCommands.batchSize,
	    plotterCommands.vertices, plotterCommands.commands, plotterCommands.transparents, plotterCommands.instances, plotterCommands.batchSize,
#ifdef WITH_IMGUI
	    imguiCommands.vertices, imguiCommands.commands, imguiCommands.transparents, imguiCommands.instances, imguiCommands.batchSize,
#endif
	    allCommands.vertices, allCommands.commands, allCommands.transparents, allCommands.instances, allCommands.batchSize);
}

void DebugOverlay::appendMoreRenderCommandsStatistics(nctl::String &string)
{
	const RenderStatistics::VaoPool &vaoPool = RenderStatistics::vaoPool();
	const RenderStatistics::Textures &textures = RenderStatistics::textures();
	const RenderStatistics::CustomBuffers &customVbos = RenderStatistics::customVBOs();
	const RenderStatistics::CustomBuffers &customIbos = RenderStatistics::customIBOs();
	const RenderStatistics::Buffers &vboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ARRAY);
	const RenderStatistics::Buffers &iboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY);
	const RenderStatistics::Buffers &uboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::UNIFORM);

	string.formatAppend(
	    "Culled nodes: %u\n"
	    "%u/%u VAOs (%u reuses, %u bindings)\n"
	    "%.2f Kb in %u Texture(s)\n"
	    "%.2f Kb in %u custom VBO(s)\n"
	    "%.2f Kb in %u custom IBO(s)\n"
	    "%.2f/%lu Kb in %u VBO(s)\n"
	    "%.2f/%lu Kb in %u IBO(s)\n"
	    "%.2f/%lu Kb in %u UBO(s)\n",
	    RenderStatistics::culled(),
	    vaoPool.size, vaoPool.capacity, vaoPool.reuses, vaoPool.bindings,
	    textures.dataSize / 1024.0f, textures.count,
	    customVbos.dataSize / 1024.0f, customVbos.count,
	    customIbos.dataSize / 1024.0f, customIbos.count,
	    vboBuffers.usedSpace / 1024.0f, vboBuffers.size / 1024, vboBuffers.count,
	    iboBuffers.usedSpace / 1024.0f, iboBuffers.size / 1024, iboBuffers.count,
	    uboBuffers.usedSpace / 1024.0f, uboBuffers.size / 1024, uboBuffers.count);
}

#ifdef WITH_LUA
void DebugOverlay::appendLuaStatistics(nctl::String &string)
{
	// Do not append statistics if there are no registered state managers
	if (LuaStatistics::numRegistered() == 0)
		return;

	string.formatAppend(
	    "%u Lua state(s) with %u tracked userdata\n"
	    "%zu Kb memory used, %d ops/s\n"
	    "Textures: %u, Sprites: %u, Mesh sprites: %u\n"
	    "Animated sprites: %u, Fonts: %u, Textnodes: %u\n"
	    "Audio buffers: %u, Buffer players: %u\n"
	    "Stream players: %u, Particle systems: %u",
	    LuaStatistics::numRegistered(), LuaStatistics::numTrackedUserDatas(),
	    LuaStatistics::usedMemory() / 1024, LuaStatistics::operations(),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::TEXTURE),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::SPRITE),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::MESH_SPRITE),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::ANIMATED_SPRITE),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::FONT),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::TEXTNODE),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::AUDIOBUFFER),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::AUDIOBUFFER_PLAYER),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::AUDIOSTREAM_PLAYER),
	    LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::PARTICLE_SYSTEM));
}
#endif

}
