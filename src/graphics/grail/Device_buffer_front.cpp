#include "grail/Device.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

Buffer::Handle Device::createBuffer(const Buffer::Desc &desc)
{
	FrontendData &frn = *frontendData_;
	Buffer::Handle handle = frn.bufferHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createBufferImpl(index, desc);
	if (success == false)
	{
		frn.bufferHandles_.destroy(handle);
		return Buffer::Handle::Invalid();
	}

	frn.bufferDescs_[index] = desc;

	return handle;
}

void Device::destroyBuffer(Buffer::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.bufferHandles_.isAlive(handle) == false)
		return;

	destroyBufferImpl(handle.index());

	frn.bufferHandles_.destroy(handle);
}

const Buffer::Desc &Device::bufferDesc(Buffer::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.bufferHandles_.isAlive(handle) == true);

	return frn.bufferDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
