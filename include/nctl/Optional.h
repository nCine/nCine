#ifndef CLASS_NCTL_OPTIONAL
#define CLASS_NCTL_OPTIONAL

#include <ncine/common_macros.h>

namespace nctl {

struct NullOptTag
{
	explicit constexpr NullOptTag(int) {}
};

constexpr NullOptTag nullopt{0};

/// A reimplementation of the standard `Optional` templated class
template <class T>
class Optional
{
  public:
	Optional() noexcept
	    : engaged_(false) {}

	Optional(NullOptTag) noexcept
	    : engaged_(false)
	{
	}

	explicit Optional(const T &value)
	    : engaged_(true)
	{
		new (&storage_) T(value);
	}

	explicit Optional(T &&value)
	    : engaged_(true)
	{
		new (&storage_) T(nctl::move(value));
	}

	Optional(const Optional &other)
	    : engaged_(other.engaged_)
	{
		if (engaged_)
			new (&storage_) T(other.value());
	}

	Optional(Optional &&other) noexcept
	    : engaged_(other.engaged_)
	{
		if (engaged_)
			new (&storage_) T(nctl::move(other.value()));
	}

	~Optional() noexcept
	{
		reset();
	}

	Optional& operator=(Optional &&other)
	{
		if (this != &other)
		{
			if (engaged_ && other.engaged_)
				value() = nctl::move(other.value());
			else if (other.engaged_)
				emplace(nctl::move(other.value()));
			else
				reset();
		}
		return *this;
	}

	Optional& operator=(const Optional &other)
	{
		if (this != &other)
		{
			if (other.engaged_)
				emplace(other.value());
			else
				reset();
		}
		return *this;
	}

	Optional& operator=(NullOptTag) noexcept
	{
		reset();
		return *this;
	}

	bool hasValue() const noexcept { return engaged_; }

	constexpr explicit operator bool() const noexcept { return engaged_; }

	T* operator->() noexcept { return ptr(); }
	const T* operator->() const noexcept { return ptr(); }

	T& operator*() & noexcept { return *ptr(); }
	const T& operator*() const& noexcept { return *ptr(); }

	T&& operator*() && noexcept { return nctl::move(*ptr()); }
	const T&& operator*() const&& noexcept { return nctl::move(*ptr()); }

	bool operator==(const Optional<T> &rhs) const
	{
		if (engaged_ != rhs.engaged_)
			return false;
		if (!engaged_)
			return true;
		return (**this == *rhs);
	}

	bool operator!=(const Optional<T> &rhs) const
	{
		return !(*this == rhs);
	}

	bool operator<(const Optional<T> &rhs) const
	{
		if (!rhs)
			return false;
		if (!*this)
			return true;
		return (**this < *rhs);
	}

	bool operator<=(const Optional<T> &rhs) const
	{
		return !(rhs < *this);
	}

	bool operator>(const Optional<T> &rhs) const
	{
		return (rhs < *this);
	}

	bool operator>=(const Optional<T> &rhs) const
	{
		return !(*this < rhs);
	}

	bool operator==(NullOptTag) const
	{
		return !engaged_;
	}

	bool operator!=(NullOptTag) const
	{
		return engaged_;
	}

	template <typename... Args>
	void emplace(Args&&... args)
		noexcept(nctl::isNoThrowConstructible<T, Args&&...>::value)
	{
		reset();
		new (&storage_) T(nctl::forward<Args>(args)...);
		engaged_ = true;
	}

	void reset() noexcept
	{
		if (engaged_)
		{
			destructObject(&value());
			engaged_ = false;
		}
	}

	T& value()
	{
		FATAL_ASSERT(engaged_);
		return *reinterpret_cast<T*>(&storage_);
	}

	const T& value() const
	{
		FATAL_ASSERT(engaged_);
		return *reinterpret_cast<const T*>(&storage_);
	}

	template <class U = typename removeCv<T>::type>
	T valueOr(U &&defaultValue) const &
	{
		return engaged_
			? *ptr()
			: T(nctl::forward<U>(defaultValue));
	}

	template <class U = typename removeCv<T>::type>
	T valueOr(U &&defaultValue) &&
	{
		return engaged_
			? nctl::move(*ptr())
			: T(nctl::forward<U>(defaultValue));
	}

	void swap(Optional &other)
	{
		using nctl::swap;
		if (engaged_ && other.engaged_)
			swap(value(), other.value());
		else if (engaged_)
		{
			other.emplace(nctl::move(value()));
			reset();
		}
		else if (other.engaged_)
		{
			emplace(nctl::move(other.value()));
			other.reset();
		}
	}

  private:
	/// Raw storage for the object, intentionally uninitialized
	alignas(T) unsigned char storage_[sizeof(T)];
	/// A flag to signal if a value is present
	bool engaged_;

	T* ptr() noexcept
	{
		FATAL_ASSERT(engaged_);
		return reinterpret_cast<T*>(storage_);
	}

	const T* ptr() const noexcept
	{
		FATAL_ASSERT(engaged_);
		return reinterpret_cast<const T*>(storage_);
	}
};

template <class T, class... Args>
Optional<T> makeOptional(Args&&... args)
{
	Optional<T> opt;
	opt.emplace(nctl::forward<Args>(args)...);
	return opt;
}

template<class T>
bool operator==(const Optional<T> &opt, const T &value)
{
	return (opt && *opt == value);
}

template<class T>
bool operator==(const T &value, const Optional<T> &opt)
{
	return (opt && value == *opt);
}

template<class T>
bool operator!=(const Optional<T> &opt, const T &value)
{
	return !(opt == value);
}

template<class T>
bool operator!=(const T &value, const Optional<T> &opt)
{
	return !(value == opt);
}

template<class T>
bool operator<(const Optional<T> &opt, const T &value)
{
	return (!opt || *opt < value);
}

template<class T>
bool operator<(const T &value, const Optional<T> &opt)
{
	return (opt && value < *opt);
}

template<class T>
bool operator<=(const Optional<T> &opt, const T &value)
{
	return !(value < opt);
}

template<class T>
bool operator<=(const T &value, const Optional<T> &opt)
{
	return !(opt < value);
}

template<class T>
bool operator>(const Optional<T> &opt, const T &value)
{
	return (value < opt);
}

template<class T>
bool operator>(const T &value, const Optional<T> &opt)
{
	return (opt < value);
}

template<class T>
bool operator>=(const Optional<T> &opt, const T &value)
{
	return !(opt < value);
}

template<class T>
bool operator>=(const T &value, const Optional<T> &opt)
{
	return !(value < opt);
}

template<class T>
bool operator==(NullOptTag, const Optional<T> &opt) noexcept
{
	return !opt.hasValue();
}

template<class T>
bool operator!=(NullOptTag, const Optional<T> &opt) noexcept
{
	return opt.hasValue();
}

}

#endif
