#ifndef CLASS_NCINE_HASH64
#define CLASS_NCINE_HASH64

#include <cstdint>
#include "common_defines.h"

namespace ncine {

/// Utility methods to calculate a uint64_t hash from strings or files
class DLL_PUBLIC Hash64
{
  public:
	/// The statistics about hashing requests
	struct Statistics
	{
		mutable unsigned int HashStringCalls = 0;
		mutable unsigned int HashedStrings = 0;
		mutable unsigned int HashedCharacters = 0;
		mutable unsigned int HashedFiles = 0;
		mutable unsigned int ScannedHashStrings = 0;
	};

	/// Returns a hash number by hashing all characters of the given strings
	uint64_t hashStrings(unsigned int count, const char **strings, const int *lengths) const;
	/// Returns a hash number by hashing all characters of the given string
	uint64_t hashString(const char *string, int length) const;
	/// Returns a hash number by hashing the date, size, and name of the given file
	uint64_t hashFileStat(const char *filename) const;
	/// Returns a hash number by scanning a hash string of the given length
	uint64_t scanHashString(const char *string, unsigned int length) const;
	/// Returns a hash number by scanning a hash string
	uint64_t scanHashString(const char *string) const;

	/// Enables or disables statitics recording
	inline void enableStatistics(bool enabled) { statisticsEnabled_ = enabled; }
	/// Returns true if statistics are recorded
	inline bool areStatisticsEnabled() const { return statisticsEnabled_; }

	/// Returns the statistics about the hashing requests
	inline const Statistics &statistics() const { return statistics_; }

	/// Resets the statistics to zero requests
	void clearStatistics();

  private:
	/// A flag that indicates if statistics are recorded
	bool statisticsEnabled_ = true;
	/// The statistics structure about the hash requests
	Statistics statistics_;
};

/// Meyers' Singleton
extern DLL_PUBLIC Hash64 &hash64();

}

#endif
