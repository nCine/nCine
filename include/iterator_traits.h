#ifndef NCINE_ITERATOR_TRAITS
#define NCINE_ITERATOR_TRAITS

namespace ncine {

struct ForwardIteratorTag { };
struct BidirectionalIteratorTag : public ForwardIteratorTag { };
struct RandomAccessIteratorTag : public BidirectionalIteratorTag { };

template <class Iterator> struct IteratorTraits { };

}

#endif
