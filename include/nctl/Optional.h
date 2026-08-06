#ifndef CLASS_NCTL_OPTIONAL
#define CLASS_NCTL_OPTIONAL

#include <ncine/common_macros.h>

namespace nctl {

struct NullOptTag
{
	explicit constexpr NullOptTag(int) {}
};

constexpr NullOptTag nullopt{0};

/// A class that manages a contained value that may or may not be present
template <class T>
class Optional
{
  public:
	/// Constructs a disengaged optional
	Optional() noexcept
	    : engaged_(false) {}

	/// Constructs a disengaged optional from a null option tag
	Optional(NullOptTag) noexcept
	    : engaged_(false)
	{
	}

	/// Constructs an optional from a copy of a value
	explicit Optional(const T &value)
	    : engaged_(true)
	{
		new (&storage_) T(value);
	}

	/// Constructs an optional moving from a value
	explicit Optional(T &&value)
	    : engaged_(true)
	{
		new (&storage_) T(nctl::move(value));
	}

	/// Copy constructor
	Optional(const Optional &other)
	    : engaged_(other.engaged_)
	{
		if (engaged_)
			new (&storage_) T(other.value());
	}

	/// Move constructor
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

	/// Move assignment operator
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

	/// Copy assignment operator
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

	/// Assigns a null option tag, resetting the optional
	Optional& operator=(NullOptTag) noexcept
	{
		reset();
		return *this;
	}

	/// Returns true if the optional currently holds a value
	bool hasValue() const noexcept { return engaged_; }

	/// Returns true if the optional currently holds a value
	constexpr explicit operator bool() const noexcept { return engaged_; }

	/// Returns a pointer to the contained value
	T* operator->() noexcept { return ptr(); }
	/// Returns a constant pointer to the contained value
	const T* operator->() const noexcept { return ptr(); }

	/// Returns a reference to the contained value
	T& operator*() & noexcept { return *ptr(); }
	/// Returns a constant reference to the contained value
	const T& operator*() const& noexcept { return *ptr(); }

	/// Returns an r-value reference to the contained value
	T&& operator*() && noexcept { return nctl::move(*ptr()); }
	/// Returns a constant r-value reference to the contained value
	const T&& operator*() const&& noexcept { return nctl::move(*ptr()); }

	/// Equality operator
	bool operator==(const Optional<T> &rhs) const
	{
		if (engaged_ != rhs.engaged_)
			return false;
		if (!engaged_)
			return true;
		return (**this == *rhs);
	}

	/// Inequality operator
	bool operator!=(const Optional<T> &rhs) const
	{
		return !(*this == rhs);
	}

	/// Less-than operator
	bool operator<(const Optional<T> &rhs) const
	{
		if (!rhs)
			return false;
		if (!*this)
			return true;
		return (**this < *rhs);
	}

	/// Less-than or equal operator
	bool operator<=(const Optional<T> &rhs) const
	{
		return !(rhs < *this);
	}

	/// Greater-than operator
	bool operator>(const Optional<T> &rhs) const
	{
		return (rhs < *this);
	}

	/// Greater-than or equal operator
	bool operator>=(const Optional<T> &rhs) const
	{
		return !(*this < rhs);
	}

	/// Equality operator with a null option tag
	bool operator==(NullOptTag) const
	{
		return !engaged_;
	}

	/// Inequality operator with a null option tag
	bool operator!=(NullOptTag) const
	{
		return engaged_;
	}

	/// Destroys the current value, if any, and constructs a new one in place
	template <typename... Args>
	void emplace(Args&&... args)
		noexcept(nctl::isNoThrowConstructible<T, Args&&...>::value)
	{
		reset();
		new (&storage_) T(nctl::forward<Args>(args)...);
		engaged_ = true;
	}

	/// Destroys the current value, if any, leaving the optional disengaged
	void reset() noexcept
	{
		if (engaged_)
		{
			destructObject(&value());
			engaged_ = false;
		}
	}

	/// Returns a reference to the contained value
	T& value()
	{
		FATAL_ASSERT(engaged_);
		return *reinterpret_cast<T*>(&storage_);
	}

	/// Returns a constant reference to the contained value
	const T& value() const
	{
		FATAL_ASSERT(engaged_);
		return *reinterpret_cast<const T*>(&storage_);
	}

	/// Returns the contained value, or a copy of the specified default if disengaged
	template <class U = typename removeCv<T>::type>
	T valueOr(U &&defaultValue) const &
	{
		return engaged_
			? *ptr()
			: T(nctl::forward<U>(defaultValue));
	}

	/// Returns the contained value moving out of it, or the specified default if disengaged
	template <class U = typename removeCv<T>::type>
	T valueOr(U &&defaultValue) &&
	{
		return engaged_
			? nctl::move(*ptr())
			: T(nctl::forward<U>(defaultValue));
	}

	/// Swaps this optional with another one
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

/// Creates an optional deducing the type and constructing the value in place
template <class T, class... Args>
Optional<T> makeOptional(Args&&... args)
{
	Optional<T> opt;
	opt.emplace(nctl::forward<Args>(args)...);
	return opt;
}

/// Equality operator between an optional and a value
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

/// Inequality operator between an optional and a value
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

/// Less-than operator between an optional and a value
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

/// Less-than or equal operator between an optional and a value
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

/// Greater-than operator between an optional and a value
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

/// Greater-than or equal operator between an optional and a value
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

/// Equality operator with a null option tag
template<class T>
bool operator==(NullOptTag, const Optional<T> &opt) noexcept
{
	return !opt.hasValue();
}

/// Inequality operator with a null option tag
template<class T>
bool operator!=(NullOptTag, const Optional<T> &opt) noexcept
{
	return opt.hasValue();
}

}

#endif
