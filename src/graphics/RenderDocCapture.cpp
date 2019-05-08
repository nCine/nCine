#if !defined(_WIN32) && !defined(__APPLE__)
	#include <dlfcn.h>
	#include "renderdoc.h"
#else
	#define NOGDI
	#include <windows.h>
	#include "renderdoc_app.h"
#endif

#include "common_macros.h"
#include "RenderDocCapture.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

namespace {
	RENDERDOC_API_1_1_2 *rdocApi = nullptr;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool RenderDocCapture::isAvailable()
{
	return (rdocApi != nullptr);
}

bool RenderDocCapture::isTargetControlConnected()
{
	if (rdocApi)
		return rdocApi->IsTargetControlConnected();
	return false;
}

bool RenderDocCapture::isFrameCapturing()
{
	if (rdocApi)
		return rdocApi->IsFrameCapturing();
	return false;
}

void RenderDocCapture::apiVersion(int *major, int *minor, int *patch)
{
	if (rdocApi)
		rdocApi->GetAPIVersion(major, minor, patch);
}

bool RenderDocCapture::isOverlayEnabled()
{
	unsigned int overlayBits = 0;
	if (rdocApi)
		overlayBits = rdocApi->GetOverlayBits();

	return overlayBits & (1 << RENDERDOC_OverlayBits::eRENDERDOC_Overlay_Enabled);
}

void RenderDocCapture::enableOverlay(bool enabled)
{
	if (rdocApi)
	{
		if (enabled)
		{
			rdocApi->MaskOverlayBits(RENDERDOC_OverlayBits::eRENDERDOC_Overlay_All,
			                         RENDERDOC_OverlayBits::eRENDERDOC_Overlay_Enabled);
		}
		else
		{
			rdocApi->MaskOverlayBits(RENDERDOC_OverlayBits::eRENDERDOC_Overlay_All &
			                             ~RENDERDOC_OverlayBits::eRENDERDOC_Overlay_Enabled,
			                         RENDERDOC_OverlayBits::eRENDERDOC_Overlay_None);
		}
	}
}

void RenderDocCapture::triggerCapture()
{
	if (rdocApi)
		rdocApi->TriggerCapture();
}

void RenderDocCapture::triggerMultiFrameCapture(uint32_t numFrames)
{
	if (rdocApi)
		rdocApi->TriggerMultiFrameCapture(numFrames);
}

uint32_t RenderDocCapture::numCaptures()
{
	if (rdocApi)
		return rdocApi->GetNumCaptures();
	return 0;
}

uint32_t RenderDocCapture::captureInfo(uint32_t idx, char *filename, uint32_t *pathlength, uint64_t *timestamp)
{
	if (rdocApi)
		return rdocApi->GetCapture(idx, filename, pathlength, timestamp);
	return 0;
}

const char *RenderDocCapture::captureFilePathTemplate()
{
	if (rdocApi)
		return rdocApi->GetCaptureFilePathTemplate();
	return nullptr;
}

void RenderDocCapture::setCaptureFilePathTemplate(const char *pathTemplate)
{
	if (rdocApi)
		rdocApi->SetCaptureFilePathTemplate(pathTemplate);
}

void RenderDocCapture::setCaptureFileComments(const char *filePath, const char *comments)
{
	if (rdocApi)
		rdocApi->SetCaptureFileComments(filePath, comments);
}

uint32_t RenderDocCapture::launchReplayUI(uint32_t connectTargetControl, const char *cmdLine)
{
	if (rdocApi)
		return rdocApi->LaunchReplayUI(connectTargetControl, cmdLine);
	return 0;
}

void RenderDocCapture::unloadCrashHandler()
{
	if (rdocApi)
		rdocApi->UnloadCrashHandler();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderDocCapture::init()
{
#if defined(_WIN32)
	if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(GetProcAddress(mod, "RENDERDOC_GetAPI"));
		const int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, reinterpret_cast<void **>(&rdocApi));
		ASSERT(ret == 1);
	}
#elif !defined(__APPLE__)
	#if defined(__ANDROID__)
	if (void *mod = dlopen("libVkLayer_GLES_RenderDoc.so", RTLD_NOW | RTLD_NOLOAD))
	#else
	if (void *mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD))
	#endif
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(dlsym(mod, "RENDERDOC_GetAPI"));
		const int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, reinterpret_cast<void **>(&rdocApi));
		ASSERT(ret == 1);
	}
#endif
}

void RenderDocCapture::shutdown()
{
	if (rdocApi)
		rdocApi->Shutdown();
}

void RenderDocCapture::startFrameCapture()
{
	if (rdocApi)
		rdocApi->StartFrameCapture(nullptr, nullptr);
}

void RenderDocCapture::endFrameCapture()
{
	if (rdocApi)
		rdocApi->EndFrameCapture(nullptr, nullptr);
}

void RenderDocCapture::discardFrameCapture()
{
	if (rdocApi)
		rdocApi->DiscardFrameCapture(nullptr, nullptr);
}

}
