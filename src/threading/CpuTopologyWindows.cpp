#include "common_windefines.h"
#include <sysinfoapi.h>
#include <errhandlingapi.h>
#include "CpuTopology.h"
#include <nctl/algorithms.h>

namespace ncine {

namespace {

struct WindowsCpuInfo
{
	int id;
	int coreId;
	int packageId;
	int efficiencyClass;
	bool isLogical;
};

int countBits(ULONG_PTR mask)
{
	int count = 0;
	while (mask)
	{
		mask &= (mask - 1);
		count++;
	}
	return count;
}

bool fillWindowsCpuInfo(nctl::Array<WindowsCpuInfo> &winCpuInfos)
{
	DWORD len = 0;
	GetLogicalProcessorInformationEx(RelationAll, NULL, &len);
	nctl::UniquePtr<char[]> buffer = nctl::makeUnique<char[]>(len);

	SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *info = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(buffer.get());
	if (!GetLogicalProcessorInformationEx(RelationAll, info, &len))
	{
		LOGW_X("GetLogicalProcessorInformationEx() failed with error: %lu", GetLastError());
		return false;
	}

	char *ptr = reinterpret_cast<char *>(info);
	const char *end = reinterpret_cast<char *>(info) + len;

	unsigned int numCpus = 0;
	while (ptr < end)
	{
		SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *entry = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)ptr;
		if (entry->Relationship == RelationProcessorCore)
		{
			PROCESSOR_RELATIONSHIP *proc = &entry->Processor;
			ULONG_PTR mask = proc->GroupMask[0].Mask;
			for (DWORD i = 0; i < sizeof(ULONG_PTR) * 8; ++i)
			{
				if (mask & ((ULONG_PTR)1 << i))
					numCpus++;
			}
		}
		ptr += entry->Size;
	}

	winCpuInfos.setCapacity(numCpus);

	unsigned int coreId = 0;
	ptr = reinterpret_cast<char *>(info);
	while (ptr < end)
	{
		SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *entry = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(ptr);
		if (entry->Relationship == RelationProcessorCore)
		{
			PROCESSOR_RELATIONSHIP *proc = &entry->Processor;
			ULONG_PTR mask = proc->GroupMask[0].Mask;
			int smtCount = countBits(mask);
			int bitIndex = 0;

			GROUP_AFFINITY *ga = proc->GroupMask;
			for (WORD g = 0; g < proc->GroupCount; g++)
			{
				KAFFINITY mask = ga[g].Mask;
				WORD group = ga[g].Group;
				for (DWORD i = 0; i < sizeof(ULONG_PTR) * 8; ++i)
				{
					if (mask & ((ULONG_PTR)1 << i))
					{
						winCpuInfos.emplaceBack();
						WindowsCpuInfo &info = winCpuInfos.back();

						info.id = i;
						info.coreId = coreId;
						info.packageId = proc->GroupMask[0].Group;
						info.efficiencyClass = proc->EfficiencyClass;
						info.isLogical = (smtCount > 1 && bitIndex > 0);
						bitIndex++;
					}
				}
				coreId++;
			}
		}

		ptr += entry->Size;
	}
}

bool sortTiers(const WindowsCpuInfo &a, const WindowsCpuInfo &b)
{
	if (a.efficiencyClass != b.efficiencyClass)
		return (a.efficiencyClass > b.efficiencyClass);
	return (a.id < b.id);
}

void fillCpuInfos(nctl::Array<WindowsCpuInfo> &winCpuInfos, nctl::Array<CpuTopology::CpuInfo> &cpuInfos)
{
	const unsigned int numCpus = winCpuInfos.size();
	cpuInfos.setCapacity(numCpus);

	nctl::sort(winCpuInfos.begin(), winCpuInfos.end(), sortTiers);
	for (unsigned int i = 0; i < winCpuInfos.size(); i++)
	{
		const WindowsCpuInfo &winInfo = winCpuInfos[i];
		cpuInfos.emplaceBack();
		CpuTopology::CpuInfo &info = cpuInfos.back();
		info.id = winInfo.id;
		info.isLogical = winInfo.isLogical;
		if (i > 0)
		{
			CpuTopology::CpuInfo &prevInfo = cpuInfos[i - 1];
			info.tier = prevInfo.tier;

			const WindowsCpuInfo &prevWinInfo = winCpuInfos[i - 1];
			if (winInfo.efficiencyClass < prevWinInfo.efficiencyClass)
				info.tier++;
		}

		LOGD_X("CPU #%02d (tier %u) - core: %d, package: %d, efficiencyClass: %d, logical: %d",
		       winInfo.id, info.tier, winInfo.coreId, winInfo.packageId, winInfo.efficiencyClass, winInfo.isLogical);
	}
}

}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool CpuTopology::discoverTopology()
{
	nctl::Array<WindowsCpuInfo> winCpuInfos;
	const bool success = fillWindowsCpuInfo(winCpuInfos);
	if (success == false)
	{
		LOGW_X("Failed to fill Windows specific CPU information");
		return false;
	}

	fillCpuInfos(winCpuInfos, cpuInfos_);
	return true;
}

}
