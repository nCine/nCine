#ifndef CLASS_NCINE_RENDERDOCCAPTURE
#define CLASS_NCINE_RENDERDOCCAPTURE

#include "common_defines.h"

namespace ncine {

/// A class for in-application integration of RenderDoc
class DLL_PUBLIC RenderDocCapture
{
  public:
	static bool isAvailable();
	static bool isTargetControlConnected();
	static bool isFrameCapturing();

	static void apiVersion(int *major, int *minor, int *patch);
	static bool isOverlayEnabled();
	static void enableOverlay(bool enabled);

	static void setCaptureTitle(const char *title);
	static void triggerCapture();
	static void triggerMultiFrameCapture(uint32_t numFrames);
	static bool endFrameCapture();
	static bool discardFrameCapture();

	static uint32_t numCaptures();
	static uint32_t captureInfo(uint32_t idx, char *filename, uint32_t *pathlength, uint64_t *timestamp);
	static const char *captureFilePathTemplate();
	static void setCaptureFilePathTemplate(const char *pathTemplate);
	static void setCaptureFileComments(const char *filePath, const char *comments);

	static uint32_t launchReplayUI(uint32_t connectTargetControl, const char *cmdLine);
	static uint32_t showReplayUI();
	static void unloadCrashHandler();

  private:
	static void init();
	static void removeHooks();

	static void startFrameCapture();

	friend class Application;
};

}

#endif
