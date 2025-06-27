#ifndef CLASS_NCINE_LOGENTRYQUEUE
#define CLASS_NCINE_LOGENTRYQUEUE

#include <nctl/Atomic.h>

namespace ncine {

/// A circular queue of entries to log from multiple threads
class LogEntryQueue
{
  public:
	/// The maximum length for a single log entry
	static const unsigned int MaxEntryLength = 1024;

	LogEntryQueue();
	~LogEntryQueue();

	/// Enqueues a log entry of the specified length
	bool enqueue(const char *entry, unsigned int length);
	/// Dequeues a log entry and copies it at the destination buffer
	unsigned int dequeue(char *dest, unsigned int maxLength);

  private:
	/// The maximum number of log entries that can be stored in the queue
	static const unsigned int MaxNumEntries = 512;
	/// The character buffer for all entries (on average an entry is shorter than its max allowed size)
	static const unsigned int BufferLength = (MaxEntryLength / 4) * MaxNumEntries;

	using LengthHeaderType = uint16_t;
	static const unsigned int LengthHeaderSize = sizeof(LengthHeaderType);
	static_assert(MaxEntryLength < LengthHeaderType(~0), "The log entry cannot be longer than the maximum number encoded in the header");

	using LengthHeaderHalfType = uint8_t;
	static const unsigned int LengthHeaderHalfSize = sizeof(LengthHeaderHalfType);

	nctl::Atomic32 readPos_;
	nctl::Atomic32 writePos_;

	char entriesBuffer_[BufferLength];

	/// Deleted copy constructor
	LogEntryQueue(const LogEntryQueue &) = delete;
	/// Deleted assignment operator
	LogEntryQueue &operator=(const LogEntryQueue &) = delete;
};

}

#endif
