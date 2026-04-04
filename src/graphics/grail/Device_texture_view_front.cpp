#include "grail/Device.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device::FrontendData
///////////////////////////////////////////////////////////

#if 0
TextureView::Handle Device::FrontendData::createTextureViewHandle(const TextureView::Desc &desc)
{
	TextureView::Handle handle = textureViewHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	textureViewDescs_[index] = desc;

	return handle;
}
#endif

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

TextureView::Handle Device::createTextureView(const TextureView::Desc &desc)
{
	FrontendData &frn = *frontendData_;
	TextureView::Handle handle = frn.textureViewHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createTextureViewImpl(index, desc);
	if (success == false)
	{
		frn.textureViewHandles_.destroy(handle);
		return TextureView::Handle::Invalid();
	}

	frn.textureViewDescs_[index] = desc;

	return handle;
}

void Device::destroyTextureView(TextureView::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.textureViewHandles_.isAlive(handle) == false)
		return;

	destroyTextureViewImpl(handle.index());

	frn.textureViewHandles_.destroy(handle);
}

const TextureView::Desc &Device::textureViewDesc(TextureView::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.textureViewHandles_.isAlive(handle) == true);

	return frn.textureViewDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
