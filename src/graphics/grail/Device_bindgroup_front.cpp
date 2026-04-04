#include "grail/Device.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

BindGroup::Handle Device::createBindGroup(const BindGroup::Desc &desc)
{
	FrontendData &frn = *frontendData_;
	BindGroup::Handle handle = frn.bindGroupHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createBindGroupImpl(index, desc);
	if (success == false)
	{
		frn.bindGroupHandles_.destroy(handle);
		return BindGroup::Handle::Invalid();
	}

	frn.bindGroupDescs_[index] = desc;

	return handle;
}

void Device::destroyBindGroup(BindGroup::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.bindGroupHandles_.isAlive(handle) == false)
		return;

	destroyBindGroupImpl(handle.index());

	frn.bindGroupHandles_.destroy(handle);
}

const BindGroup::Desc &Device::bindGroupDesc(BindGroup::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.bindGroupHandles_.isAlive(handle) == true);

	return frn.bindGroupDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
