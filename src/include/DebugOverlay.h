#ifndef CLASS_NCINE_DEBUGOVERLAY
#define CLASS_NCINE_DEBUGOVERLAY

#include "IDebugOverlay.h"
#include "nctl/UniquePtr.h"
#include "nctl/String.h"
#include "ProfilePlotter.h" // for `~UniquePtr()`

namespace ncine {

class AppConfiguration;
class Font;
class TextNode;

/// Overlay info texts and graphs for debug
class DebugOverlay : public IDebugOverlay
{
  public:
	DebugOverlay(const AppConfiguration &appCfg);

	void update() override;
	void updateFrameTimings() override;

  private:
	/// Maximum length for the information strings
	static const unsigned int MaxTextLength = 256;

	bool plotterInitialized_;
	bool infoTextInitialized_;

	nctl::UniquePtr<ProfilePlotter> profilePlotter_;
	nctl::UniquePtr<Font> font_;
	nctl::UniquePtr<TextNode> infoLineTopLeft_;
	nctl::UniquePtr<TextNode> infoLineTopRight_;
	nctl::UniquePtr<TextNode> infoLineBottomLeft_;
	nctl::UniquePtr<TextNode> infoLineBottomRight_;
	nctl::String infoStringTopLeft_;
	nctl::String infoStringTopRight_;
	nctl::String infoStringBottomRight_;

	void initPlotter();
	void initInfoText();
	void appendRenderCommandsStatistics(nctl::String &string);
	void appendMoreRenderCommandsStatistics(nctl::String &string);
#ifdef WITH_LUA
	void appendLuaStatistics(nctl::String &string);
#endif

	/// Deleted copy constructor
	DebugOverlay(const DebugOverlay &) = delete;
	/// Deleted assignment operator
	DebugOverlay &operator=(const DebugOverlay &) = delete;
};

}

#endif
