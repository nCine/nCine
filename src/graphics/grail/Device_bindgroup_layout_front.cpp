#include "grail/Device.h"
#include "grail/private/hash_functions.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

BindGroupLayout::Handle Device::createBindGroupLayout(const BindGroupLayout::CreateDesc &desc)
{
	FrontendData &frn = *frontendData_;
	const uint64_t hashValue = hashDesc(desc);

	BindGroupLayout::Handle *handlePtr = frn.bindGroupLayoutFromHashToHandle_.find(hashValue);
	// If another bind group layout with the same hash already exists, don't create a new one
	if (handlePtr != nullptr)
	{
		ASSERT(frn.bindGroupLayoutHandles_.isAlive(*handlePtr) == true);
		return *handlePtr;
	}

	BindGroupLayout::Handle handle = frn.bindGroupLayoutHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createBindGroupLayoutImpl(index, desc);
	if (success == false)
	{
		frn.bindGroupLayoutHandles_.destroy(handle);
		return BindGroupLayout::Handle::Invalid();
	}

	frn.bindGroupLayoutFromHashToHandle_.insert(hashValue, handle);
	frn.bindGroupLayoutFromHandleToHash_.insert(handle, hashValue);

	BindGroupLayout::Desc &descOut = frn.bindGroupLayoutDescs_[index];
	descOut.entries.setCapacity(desc.entryCount);
	for (unsigned int i = 0; i < desc.entryCount; i++)
		descOut.entries.pushBack(desc.entries[i]);

	return handle;
}

void Device::destroyBindGroupLayout(BindGroupLayout::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.bindGroupLayoutHandles_.isAlive(handle) == false)
		return;

	destroyBindGroupLayoutImpl(handle.index());

	const uint64_t hashValue = frn.bindGroupLayoutFromHandleToHash_[handle];
	frn.bindGroupLayoutFromHashToHandle_.remove(hashValue);
	frn.bindGroupLayoutFromHandleToHash_.remove(handle);

	frn.bindGroupLayoutHandles_.destroy(handle);
}

const BindGroupLayout::Desc &Device::bindGroupLayoutDesc(BindGroupLayout::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.bindGroupLayoutHandles_.isAlive(handle) == true);

	return frn.bindGroupLayoutDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
