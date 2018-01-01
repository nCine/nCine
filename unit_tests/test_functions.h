#ifndef TEST_FUNCTIONS
#define TEST_FUNCTIONS

namespace {

template <int value>
class AddValue
{
  public:
	inline void operator()(int &n) const { n += value; }
};

template <int value>
inline void addValue(int &n) { n += value; }

template <int value>
inline int copyAddValue(const int &n) { return n + value; }

class GenerateSequenceFrom
{
  public:
	explicit GenerateSequenceFrom(int n) : value_(n) { }
	inline int operator()() { return value_++; }

  private:
	int value_;
};

}

#endif
