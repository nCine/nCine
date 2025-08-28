#ifndef TEST_OBJECT_POLICY_TAG_H
#define TEST_OBJECT_POLICY_TAG_H

#include <nctl/utility.h>

namespace {

/// Both movable and copyable
class Movable
{
  public:
	Movable() {}

	Movable(const Movable &) = default;
	Movable(Movable &&) = default;
	Movable &operator=(const Movable &) = default;
	Movable &operator=(Movable &&) = default;
};

static_assert(nctl::isSame<nctl::detail::ObjectPolicyTag<Movable>::type, nctl::detail::MovableTag>::value,
              "Movable should map to MovableTag");

/// Movable only, not copyable
class MoveOnly
{
  public:
	MoveOnly() {}

	MoveOnly(MoveOnly &&) = default;
	MoveOnly &operator=(MoveOnly &&) = default;

  private:
	MoveOnly(const MoveOnly &) = delete;
	MoveOnly &operator=(const MoveOnly &) = delete;
};

static_assert(nctl::isSame<nctl::detail::ObjectPolicyTag<MoveOnly>::type, nctl::detail::MoveOnlyTag>::value,
              "MoveOnly should map to MoveOnlyTag");

/// Copyable but not movable
class Copyable
{
  public:
	Copyable() {}

	Copyable(const Copyable &) = default;
	Copyable &operator=(const Copyable &) = default;

  private:
	Copyable(Copyable &&) = delete;
	Copyable &operator=(Copyable &&) = delete;
};

static_assert(nctl::isSame<nctl::detail::ObjectPolicyTag<Copyable>::type, nctl::detail::CopyableTag>::value,
              "Copyable should map to CopyableTag");

/// Trivially copyable
class Trivial
{
  public:
	int num_;
};

static_assert(nctl::isSame<nctl::detail::ObjectPolicyTag<Trivial>::type, nctl::detail::TrivialTag>::value,
              "Trivial should map to TrivialTag");

/// Not movable and not copyable
class NonMovable
{
  public:
	NonMovable() {}

  private:
	NonMovable(const NonMovable &) = delete;
	NonMovable(NonMovable &&) = delete;
	NonMovable &operator=(const NonMovable &) = delete;
	NonMovable &operator=(NonMovable &&) = delete;
};

}

#endif
