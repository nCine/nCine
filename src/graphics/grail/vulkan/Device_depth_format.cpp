#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

const VkFormat DepthCandidatesHigh[] = {
	// non-stencil
	VK_FORMAT_D32_SFLOAT,
	VK_FORMAT_X8_D24_UNORM_PACK32,
	VK_FORMAT_D16_UNORM,
	// stencil fallback
	VK_FORMAT_D32_SFLOAT_S8_UINT,
	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D16_UNORM_S8_UINT
};

const VkFormat DepthCandidatesMedium[] = {
	// non-stencil
	VK_FORMAT_X8_D24_UNORM_PACK32,
	VK_FORMAT_D32_SFLOAT,
	VK_FORMAT_D16_UNORM,
	// stencil fallback
	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D32_SFLOAT_S8_UINT,
	VK_FORMAT_D16_UNORM_S8_UINT
};

const VkFormat DepthCandidatesLow[] = {
	// non-stencil
	VK_FORMAT_D16_UNORM,
	VK_FORMAT_X8_D24_UNORM_PACK32,
	VK_FORMAT_D32_SFLOAT,
	// stencil fallback
	VK_FORMAT_D16_UNORM_S8_UINT,
	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D32_SFLOAT_S8_UINT
};

bool hasStencil(VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default:
			return false;
	}
}

bool hasFlag(Device::DepthFormatQuery::Usage value, Device::DepthFormatQuery::Usage flag)
{
	return (uint8_t(value) & uint8_t(flag)) != 0;
}

VkFormatFeatureFlags queryToFormatFeatureFlags(const Device::DepthFormatQuery &query)
{
	VkFormatFeatureFlags flags = 0;

	const bool sampled =
		hasFlag(query.usage, Device::DepthFormatQuery::Usage::SAMPLED) ||
		hasFlag(query.usage, Device::DepthFormatQuery::Usage::FILTERABLE);

	if (hasFlag(query.usage, Device::DepthFormatQuery::Usage::DEPTH_STENCIL_ATTACHMENT))
		flags |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

	if (sampled)
		flags |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

	if (hasFlag(query.usage, Device::DepthFormatQuery::Usage::FILTERABLE))
		flags |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	if (hasFlag(query.usage, Device::DepthFormatQuery::Usage::TRANSFER_SRC))
		flags |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;

	if (hasFlag(query.usage, Device::DepthFormatQuery::Usage::TRANSFER_DST))
		flags |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

	return flags;
}

}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

Format Device::findDepthFormatImpl(const DepthFormatQuery &query)
{
	BackendData &bck = *backendData_;

	const VkFormat *candidates = nullptr;
	uint32_t count = 0;

	switch (query.depthPrecision)
	{
		default:
		case DepthFormatQuery::DepthPrecision::HIGH:
			candidates = DepthCandidatesHigh;
			count = nctl::arraySize(DepthCandidatesHigh);
			break;
		case DepthFormatQuery::DepthPrecision::MEDIUM:
			candidates = DepthCandidatesMedium;
			count = nctl::arraySize(DepthCandidatesMedium);
			break;
		case DepthFormatQuery::DepthPrecision::LOW:
			candidates = DepthCandidatesLow;
			count = nctl::arraySize(DepthCandidatesLow);
			break;
	}

	VkFormatFeatureFlags required = queryToFormatFeatureFlags(query);

	for (uint32_t i = 0; i < count; i++)
	{
		const VkFormat format = candidates[i];

		// Enforce stencil requirement only if explicitly requested
		if (query.requireStencil && hasStencil(format) == false)
			continue;

		const VkFormatProperties *propsPtr = bck.depthFormatProperties_.find(format);
		if (propsPtr == nullptr)
			continue;

		const VkFormatProperties &props = *propsPtr;
		const VkFormatFeatureFlags features = props.optimalTilingFeatures;

		if ((features & required) != required)
			continue;

		return vlkFormatToGrl(format);
	}

	return Format::UNDEFINED;
}

} // namespace grail
} // namespace ncine
