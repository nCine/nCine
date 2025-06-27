#include <cstring> // for `memcpy()`
#include <nctl/algorithms.h>
#include "common_macros.h"
#include "LogEntryQueue.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LogEntryQueue::LogEntryQueue()
{
	memset(entriesBuffer_, 0, BufferLength);
}

LogEntryQueue::~LogEntryQueue()
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \note Multiple producers */
bool LogEntryQueue::enqueue(const char *entry, unsigned int length)
{
	ASSERT_NOLOG(entry != nullptr);
	ASSERT_NOLOG(length <= MaxEntryLength - 1); // Leave room for null-termination when copying out on dequeue
	if (entry == nullptr || length == 0 || length > BufferLength || length > MaxEntryLength - 1 || length > uint16_t(-1))
		return false;

	const uint32_t currentWritePos = writePos_.load(nctl::MemoryModel::RELAXED);
	const uint32_t currentReadPos = readPos_.load(nctl::MemoryModel::ACQUIRE);
	const uint32_t used = (currentWritePos >= currentReadPos)
	                          ? (currentWritePos - currentReadPos)
	                          : (BufferLength - (currentReadPos - currentWritePos));
	const uint32_t free = BufferLength - used;

	if (free <= LengthHeaderSize + length)
		return false; // Buffer queue is full

	const uint32_t reservedPos = writePos_.fetchAdd(LengthHeaderSize + length, nctl::MemoryModel::RELAXED);
	const uint32_t nextWritePos = (reservedPos + LengthHeaderSize + length) % BufferLength;

	// Write header
	entriesBuffer_[reservedPos % BufferLength] = static_cast<uint8_t>(length & 0xFF);
	entriesBuffer_[(reservedPos + 1) % BufferLength] = static_cast<uint8_t>((length >> 8) & 0xFF);

	const uint32_t entryStart = (reservedPos + LengthHeaderSize) % BufferLength;
	const uint32_t entryChunkLength = nctl::min(length, BufferLength - entryStart);
	memcpy(&entriesBuffer_[entryStart], entry, entryChunkLength);
	if (entryChunkLength < length)
		memcpy(&entriesBuffer_[0], entry + entryChunkLength, length - entryChunkLength);

	writePos_.store(nextWritePos, nctl::MemoryModel::RELEASE);
	return true;
}

/*! \note Single consumer
 * \returns The length of the entry, clamped at the maximum length passed */
unsigned int LogEntryQueue::dequeue(char *dest, unsigned int maxLength)
{
	const uint32_t currentReadPos = readPos_.load(nctl::MemoryModel::RELAXED);
	const uint32_t currentWritePos = writePos_.load(nctl::MemoryModel::ACQUIRE);
	if (currentReadPos == currentWritePos)
		return 0; // Buffer queue is empty

	// Read header safely across wrap
	uint8_t low = entriesBuffer_[currentReadPos % BufferLength];
	uint8_t high = entriesBuffer_[(currentReadPos + 1) % BufferLength];
	const LengthHeaderType length = static_cast<LengthHeaderType>(low | (high << 8));

	const uint32_t entryStart = (currentReadPos + LengthHeaderSize) % BufferLength;
	const uint32_t charsToCopy = (length < maxLength) ? length : maxLength - 1;

	const uint32_t entryChunkLength = nctl::min(charsToCopy, BufferLength - entryStart);
	memcpy(dest, &entriesBuffer_[entryStart], entryChunkLength);
	if (entryChunkLength < charsToCopy)
		memcpy(dest + entryChunkLength, &entriesBuffer_[0], charsToCopy - entryChunkLength);

	dest[charsToCopy] = '\0';

	uint32_t nextReadPos = currentReadPos + LengthHeaderSize + length;
	if (nextReadPos >= BufferLength)
		nextReadPos -= BufferLength;

	readPos_.store(nextReadPos, nctl::MemoryModel::RELEASE);
	return charsToCopy;
}

}
