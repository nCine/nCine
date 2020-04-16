#include "gtest_uniqueptr.h"
#include "test_thread_functions.h"

namespace {

const int NumThreads = 100;
const int NumIterations = 1000;

class SharedPtrThreadsTest : public ::testing::Test
{
  public:
	SharedPtrThreadsTest()
	    : ptr_(newObject<int>(Value)), tr_(this) {}

	nctl::SharedPtr<int> ptr_;
	ThreadRunner<NumThreads> tr_;
};

TEST_F(SharedPtrThreadsTest, ShareAssignmentMultithread)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet {
		SharedPtrThreadsTest *obj = static_cast<SharedPtrThreadsTest *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			nctl::SharedPtr<int> newPtr(obj->ptr_);
		}
		return static_cast<SharedPtrThreadsTest *>(arg)->tr_.retFunc();
	});

	printPointer("Letting all threads join and new shared pointer go out of scope, ", ptr_);
	ASSERT_EQ(ptr_.useCount(), 1);
}

}
