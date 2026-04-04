#include "grail/Device.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device::FrontendData
///////////////////////////////////////////////////////////

Texture::Handle Device::FrontendData::createTextureHandle(const Texture::Desc &desc)
{
	Texture::Handle handle = textureHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	textureDescs_[index] = desc;

	return handle;
}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

Texture::Handle Device::createTexture(const Texture::Desc &desc)
{
	FrontendData &frn = *frontendData_;
	Texture::Handle handle = frn.textureHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createTextureImpl(index, desc);
	if (success == false)
	{
		frn.textureHandles_.destroy(handle);
		return Texture::Handle::Invalid();
	}

	frn.textureDescs_[index] = desc;

	return handle;
}

void Device::destroyTexture(Texture::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.textureHandles_.isAlive(handle) == false)
		return;

	destroyTextureImpl(handle.index());

	frn.textureHandles_.destroy(handle);
}

const Texture::Desc &Device::textureDesc(Texture::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.textureHandles_.isAlive(handle) == true);

	return frn.textureDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
