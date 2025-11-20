#include <cstdio>
#include <ctype.h>
#include "CpuTopology.h"
#include <nctl/algorithms.h>

namespace ncine {

namespace {

struct LinuxCpuInfo
{
	int id;
	int coreId;
	int clusterId;
	int packageId;
	int capacity;
	int maxFreqKhz;
	bool isLogical;
};

const unsigned int MaxCpus = 256;

const unsigned int BufferSize = 256;
char buffer[BufferSize];
const unsigned int MaxPath = 256;
char path[MaxPath];

nctl::Array<unsigned char> cpuPresentArray;

bool readFile(const char *path, char *buffer, size_t size)
{
	FILE *fp = fopen(path, "r");
	if (!fp)
		return false;

	const size_t len = fread(buffer, 1, size - 1, fp);
	fclose(fp);

	if (len == 0 || len >= size)
		return false;

	buffer[len] = '\0';
	return true;
}

// Parse ranges like "0-3,5,7-8"
void parseCpuRangeList(const char *str, nctl::Array<unsigned char> &cpuPresentArray)
{
	const char *p = str;
	while (*p)
	{
		while (isspace(static_cast<unsigned char>(*p)) || *p == ',')
			p++;

		if (!isdigit(static_cast<unsigned char>(*p)))
			break;

		char *endptr = nullptr;
		const long start = strtol(p, &endptr, 10);
		long end = start;
		p = endptr;

		if (*p == '-')
		{
			p++;
			end = strtol(p, &endptr, 10);
			p = endptr;
		}

		if (start >= 0 && end < MaxCpus && start <= end)
		{
			for (long i = start; i <= end; i++)
				cpuPresentArray.pushBack(i);
		}

		while (*p && *p != ',')
			p++;
	}
}

int parseInt(const char *str)
{
	const char *p = str;
	while (*p && !isdigit((unsigned char)*p))
		p++;
	return atoi(p);
}

bool readCpuPresent(nctl::Array<unsigned char> &cpuPresentArray)
{
	if (readFile("/sys/devices/system/cpu/present", buffer, sizeof(buffer)) == false)
		return false;

	parseCpuRangeList(buffer, cpuPresentArray);
	return true;
}

bool fillLinuxCpuInfo(LinuxCpuInfo &info, unsigned char cpuId)
{
	info.id = cpuId;

	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/topology/core_id", cpuId);
	if (readFile(path, buffer, sizeof(buffer)) == false)
		return false;
	info.coreId = parseInt(buffer);

	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/topology/cluster_id", cpuId);
	if (readFile(path, buffer, sizeof(buffer)) == false)
		info.clusterId = 65535;
	else
		info.clusterId = parseInt(buffer);

	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/topology/physical_package_id", cpuId);
	if (readFile(path, buffer, sizeof(buffer)) == false)
		info.packageId = 0;
	else
		info.packageId = parseInt(buffer);

	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/topology/thread_siblings_list", cpuId);
	if (readFile(path, buffer, sizeof(buffer)) == false)
		info.isLogical = false;
	else
	{
		const int firstSibling = parseInt(buffer); // parsing only the first int from the list
		info.isLogical = (firstSibling != cpuId);
	}

	// `cpu_capacity` is not always available, but ARM usually has it
	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cpu_capacity", cpuId);
	if (readFile(path, buffer, sizeof(buffer)) == false)
		info.capacity = 1024; // fallback to the highest value
	else
		info.capacity = parseInt(buffer);

	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_max_freq", cpuId);
	if (readFile(path, buffer, sizeof(buffer)) == false)
		info.maxFreqKhz = 0; // fallback to zero, no sorting based on frequency
	else
		info.maxFreqKhz = parseInt(buffer);

	return true;
}

bool sortTiers(const LinuxCpuInfo &a, const LinuxCpuInfo &b)
{
	if (a.capacity != b.capacity)
		return (a.capacity > b.capacity);
	if (a.maxFreqKhz != b.maxFreqKhz)
		return (a.maxFreqKhz > b.maxFreqKhz);
	return (a.id < b.id);
}

void fillCpuInfos(nctl::Array<LinuxCpuInfo> &linuxCpuInfos, nctl::Array<CpuTopology::CpuInfo> &cpuInfos)
{
	const unsigned int numPresentCpus = linuxCpuInfos.size();
	cpuInfos.setCapacity(numPresentCpus);

	nctl::sort(linuxCpuInfos.begin(), linuxCpuInfos.end(), sortTiers);
	for (unsigned int i = 0; i < linuxCpuInfos.size(); i++)
	{
		const LinuxCpuInfo &linuxInfo = linuxCpuInfos[i];
		cpuInfos.emplaceBack();
		CpuTopology::CpuInfo &info = cpuInfos.back();
		info.id = linuxInfo.id;
		info.isLogical = linuxInfo.isLogical;
		if (i > 0)
		{
			CpuTopology::CpuInfo &prevInfo = cpuInfos[i - 1];
			info.tier = prevInfo.tier;

			const LinuxCpuInfo &prevLinuxInfo = linuxCpuInfos[i - 1];
			if (linuxInfo.capacity < prevLinuxInfo.capacity || linuxInfo.maxFreqKhz < prevLinuxInfo.maxFreqKhz)
				info.tier++;
		}

		LOGD_X("CPU #%02d (tier %u) - core: %d, cluster: %d, package: %d, capacity: %d, max freq: %d Mhz, logical: %d",
		       linuxInfo.id, info.tier, linuxInfo.coreId, linuxInfo.clusterId, linuxInfo.packageId, linuxInfo.capacity,
		       linuxInfo.maxFreqKhz / 1024, linuxInfo.isLogical);
	}
}

}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool CpuTopology::discoverTopology()
{
	nctl::Array<unsigned char> cpuPresentArray(64);

	const bool success = readCpuPresent(cpuPresentArray);
	if (success == false)
	{
		LOGW_X("Failed to read CPU present info from sysfs");
		return false;
	}

	const unsigned int numPresentCpus = cpuPresentArray.size();
	nctl::Array<LinuxCpuInfo> linuxCpuInfos(numPresentCpus);
	for (unsigned int i = 0; i < numPresentCpus; i++)
	{
		const unsigned char cpuId = cpuPresentArray[i];
		linuxCpuInfos.emplaceBack();
		const bool success = fillLinuxCpuInfo(linuxCpuInfos.back(), cpuId);
		if (success == false)
		{
			LOGW_X("Failed to fill Linux specific CPU information");
			return false;
		}
	}

	fillCpuInfos(linuxCpuInfos, cpuInfos_);
	return true;
}

}
