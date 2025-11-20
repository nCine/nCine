#ifndef CLASS_NCINE_CPUTOPOLOGY
#define CLASS_NCINE_CPUTOPOLOGY

#include <nctl/Array.h>

namespace ncine {

/// A class to discover the CPU topology
class CpuTopology
{
  public:
	struct CpuInfo
	{
		unsigned int id = 0;
		bool isLogical = false;
		unsigned int tier = 0;
	};

	CpuTopology();

	inline unsigned int numTotalCores() const { return numTotalCores_; }
	inline unsigned int numPhysicalCores() const { return numPhysicalCores_; }
	inline unsigned int numTiers() const { return numTiers_; }
	const CpuInfo &cpuInfo(unsigned int idx) const;

  private:
	unsigned int numTotalCores_;
	unsigned int numPhysicalCores_;
	unsigned int numTiers_;
	nctl::Array<CpuInfo> cpuInfos_;

	bool discoverTopology();
	bool discoverDummyTopology();
};

}

#endif
