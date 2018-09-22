#ifndef CLASS_NCINE_IDEBUGOVERLAY
#define CLASS_NCINE_IDEBUGOVERLAY

#include "AppConfiguration.h"
#include "Timer.h"

namespace ncine {

/// Interface for debug overlays
class DLL_PUBLIC IDebugOverlay
{
  public:
	struct DisplaySettings
	{
		DisplaySettings()
			: showProfilerGraphs(true), showInfoText(true), showInterface(false) { }

		/// True if showing the profiler graphs
		bool showProfilerGraphs;
		/// True if showing the information text
		bool showInfoText;
		/// True if showing the debug interface (ImGui only)
		bool showInterface;
	};

	IDebugOverlay(const AppConfiguration &appCfg);
	virtual ~IDebugOverlay();

	inline DisplaySettings &settings() { return settings_; }
	virtual void update() = 0;
	virtual void updateFrameTimings() = 0;

  protected:
	DisplaySettings settings_;
	Timer updateTimer_;
	float updateTime_;

	/// Deleted copy constructor
	IDebugOverlay(const IDebugOverlay &) = delete;
	/// Deleted assignment operator
	IDebugOverlay &operator=(const IDebugOverlay &) = delete;
};

inline IDebugOverlay::IDebugOverlay(const AppConfiguration &appCfg)
	: updateTime_(appCfg.profileTextUpdateTime())
{
	updateTimer_.start();
}

inline IDebugOverlay::~IDebugOverlay()
{

}

}

#endif
