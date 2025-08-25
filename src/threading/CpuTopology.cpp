#include "CpuTopology.h"
#include "Thread.h"
#include <nctl/algorithms.h>

namespace ncine {

namespace {

	bool sortTiersAndLogical(const CpuTopology::CpuInfo &a, const CpuTopology::CpuInfo &b)
	{
		if (a.isLogical != b.isLogical)
			return (a.isLogical ? false : true);
		if (a.tier != b.tier)
			return (a.tier < b.tier);
		return (a.id < b.id);
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

CpuTopology::CpuTopology()
    : numTotalCores_(0), numPhysicalCores_(0), numTiers_(0)
{
	bool success = discoverTopology();
	if (success == false)
		discoverDummyTopology();

	nctl::sort(cpuInfos_.begin(), cpuInfos_.end(), sortTiersAndLogical);

	numTotalCores_ = cpuInfos_.size();
	numPhysicalCores_ = 0;
	unsigned int maxTier = 0;
	for (unsigned int i = 0; i < cpuInfos_.size(); i++)
	{
		LOGD_X("CPU #%02u (tier: %u) - logical: %d", cpuInfos_[i].id, cpuInfos_[i].tier, cpuInfos_[i].isLogical);

		if (cpuInfos_[i].isLogical == false)
			numPhysicalCores_++;
		if (cpuInfos_[i].tier > maxTier)
			maxTier = cpuInfos_[i].tier;
	}
	numTiers_ = maxTier + 1;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const CpuTopology::CpuInfo &CpuTopology::cpuInfo(unsigned int idx) const
{
	ASSERT(idx < cpuInfos_.size());
	if (idx <= cpuInfos_.size())
		return cpuInfos_[idx];
	return cpuInfos_[0];
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef __EMSCRIPTEN__
bool CpuTopology::discoverTopology()
{
	return discoverDummyTopology();
}
#endif

bool CpuTopology::discoverDummyTopology()
{
	const unsigned int numProcessors = Thread::numProcessors();
	numTotalCores_ = numProcessors;
	numPhysicalCores_ = numProcessors;
	numTiers_ = 1;

	cpuInfos_.clear();
	cpuInfos_.setCapacity(numProcessors);
	for (unsigned int i = 0; i < numProcessors; i++)
	{
		cpuInfos_.emplaceBack();
		CpuInfo &info = cpuInfos_.back();
		info.id = i;
		info.isLogical = false;
		info.tier = 0;
	}

	return true;
}

}
