#ifndef GRAIL_HASHTYPES
#define GRAIL_HASHTYPES

#include <nctl/HashMap.h>
#include "grail/Handle.h"

namespace ncine {
namespace grail {

template <class T>
using Desc64HashMap = nctl::HashMap<uint64_t, T, nctl::Mul64To32Hash<uint64_t>>;

template <class Tag>
using GrlHandleHashMap = nctl::HashMap<Handle<Tag>, uint64_t, HandleHash<Tag>>;

} // namespace grail
} // namespace ncine

#endif
