#ifndef CLASS_NCTL_ALLOCMANAGER
#define CLASS_NCTL_ALLOCMANAGER

#include <ncine/common_defines.h>

namespace nctl {

class IAllocator;

/// Allocator manager initializer
class DLL_PUBLIC AllocManagerInitializer
{
  public:
	AllocManagerInitializer();
	~AllocManagerInitializer();
};

/// The allocators manager
class DLL_PUBLIC AllocManager
{
  public:
	inline IAllocator &defaultAllocator() { return *defaultAllocator_; }
	inline IAllocator &stringAllocator() { return *stringAllocator_; }

	IAllocator *setDefaultAllocator(IAllocator *allocator);
	IAllocator *setStringAllocator(IAllocator *allocator);

  private:
	IAllocator *defaultAllocator_;
	IAllocator *stringAllocator_;

	AllocManager();
	~AllocManager();

	friend AllocManager DLL_PUBLIC &theAllocManager();
	friend class AllocManagerInitializer;
};

extern DLL_PUBLIC AllocManager &theAllocManager();

extern DLL_PUBLIC IAllocator &theDefaultAllocator();
extern DLL_PUBLIC IAllocator &theStringAllocator();
extern DLL_PUBLIC IAllocator &theGlfwAllocator();
extern DLL_PUBLIC IAllocator &theSdl2Allocator();
extern DLL_PUBLIC IAllocator &theImGuiAllocator();
extern DLL_PUBLIC IAllocator &theNuklearAllocator();
extern DLL_PUBLIC IAllocator &theLuaAllocator();

}

#endif
