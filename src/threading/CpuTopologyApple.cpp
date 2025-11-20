#include <sys/sysctl.h>
#include "CpuTopology.h"
#include <nctl/algorithms.h>

namespace ncine {

namespace {

struct AppleCpuInfo
{
	int id;
	int coreId;
	int efficiencyClass;
	bool isLogical;
};

// Helper function to read integer sysctl values
int sysctlInt(const char *name)
{
	int value = 0;
	size_t size = sizeof(value);
	if (sysctlbyname(name, &value, &size, nullptr, 0) != 0)
		return 0;
	return value;
}

bool sortTiers(const AppleCpuInfo &a, const AppleCpuInfo &b)
{
	if (a.efficiencyClass != b.efficiencyClass)
		return (a.efficiencyClass > b.efficiencyClass);
	return (a.id < b.id);
}

bool fillAppleCpuInfo(nctl::Array<AppleCpuInfo> &appleCpuInfos)
{
	const int numLogical = sysctlInt("hw.logicalcpu");
	const int numPhysical = sysctlInt("hw.physicalcpu");
	const int threadsPerCore = numLogical / numPhysical;

	if (numLogical == 0 || numPhysical == 0)
		return false;

	appleCpuInfos.setCapacity(numLogical);

	const int pCores = sysctlInt("hw.perflevel0.physicalcpu");
	const int eCores = sysctlInt("hw.perflevel1.physicalcpu");

	if (pCores == 0 && eCores == 0)
		return false;

	int logicalId = 0;
	int coreId = 0;
	for (int i = 0; i < eCores; ++i)
	{
		for (int t = 0; t < threadsPerCore; ++t)
		{
			appleCpuInfos.emplaceBack();
			AppleCpuInfo &info = appleCpuInfos.back();
			info.id = logicalId++;
			info.coreId = coreId;
			info.efficiencyClass = 0;
			info.isLogical = (t > 0);
		}
		++coreId;
	}

	for (int i = 0; i < pCores; ++i)
	{
		for (int t = 0; t < threadsPerCore; ++t)
		{
			appleCpuInfos.emplaceBack();
			AppleCpuInfo &info = appleCpuInfos.back();
			info.id = logicalId++;
			info.coreId = coreId;
			info.efficiencyClass = 1;
			info.isLogical = (t > 0);
		}
		++coreId;
	}

	return true;
}

void fillCpuInfos(nctl::Array<AppleCpuInfo> &appleCpuInfos, nctl::Array<CpuTopology::CpuInfo> &cpuInfos)
{
	const unsigned int numPresentCpus = appleCpuInfos.size();
	cpuInfos.setCapacity(numPresentCpus);

	nctl::sort(appleCpuInfos.begin(), appleCpuInfos.end(), sortTiers);
	for (unsigned int i = 0; i < appleCpuInfos.size(); i++)
	{
		const AppleCpuInfo &appleInfo = appleCpuInfos[i];
		cpuInfos.emplaceBack();
		CpuTopology::CpuInfo &info = cpuInfos.back();
		info.id = appleInfo.id;
		info.isLogical = appleInfo.isLogical;
		if (i > 0)
		{
			CpuTopology::CpuInfo &prevInfo = cpuInfos[i - 1];
			info.tier = prevInfo.tier;

			const AppleCpuInfo &prevAppleInfo = appleCpuInfos[i - 1];
			if (appleInfo.efficiencyClass < prevAppleInfo.efficiencyClass)
				info.tier++;
		}

		LOGD_X("CPU #%02d (tier %u) - core: %d, efficiencyClass: %d, logical: %d",
		       appleInfo.id, info.tier, appleInfo.coreId, appleInfo.efficiencyClass, appleInfo.isLogical);
	}
}

}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool CpuTopology::discoverTopology()
{
	nctl::Array<AppleCpuInfo> appleCpuInfos;
	const bool success = fillAppleCpuInfo(appleCpuInfos);
	if (success == false)
	{
		LOGW_X("Failed to fill Apple specific CPU information");
		return false;
	}

	fillCpuInfos(appleCpuInfos, cpuInfos_);
	return true;
}

}
