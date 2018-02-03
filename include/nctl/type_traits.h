#ifndef NCTL_TYPETRAITS
#define NCTL_TYPETRAITS

namespace nctl {

template <class T> struct removeReference { using type = T; };
template <class T> struct removeReference<T &> { using type = T; };
template <class T> struct removeReference<T &&> { using type = T; };

template <class T> struct isLValueReference { static constexpr bool value = false; };
template <class T> struct isLValueReference<T &> { static constexpr bool value = true; };

}

#endif
