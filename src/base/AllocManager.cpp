#include <new>
#include <ncine/allocators_config.h>
#include <nctl/AllocManager.h>
#include <nctl/MallocAllocator.h>
#include <nctl/FreeListAllocator.h>
#include <nctl/ProxyAllocator.h>

#ifdef WITH_IMGUI
	#include "imgui.h"
#endif

#ifdef _MSC_VER
	#pragma warning(disable : 4075)
	#pragma init_seg(".CRT$XCT")
#endif

namespace nctl {

#ifdef _MSC_VER
static AllocManagerInitializer allocManagerInit;
#else
static AllocManagerInitializer allocManagerInit __attribute__((init_priority(101)));
#endif

alignas(sizeof(AllocManager)) static uint8_t allocManagerBuffer[sizeof(AllocManager)];
static AllocManager &allocManager = reinterpret_cast<AllocManager &>(allocManagerBuffer);
static IAllocator *mainAllocator = nullptr;

#ifdef USE_FREELIST
static const unsigned int FreeListSize = FREELIST_BUFFER;
alignas(IAllocator::DefaultAlignment) static uint8_t freelistMemory[FreeListSize];
alignas(IAllocator::DefaultAlignment) static uint8_t freelistAllocatorBuffer[sizeof(FreeListAllocator)];
static FreeListAllocator &freelistAllocator = reinterpret_cast<FreeListAllocator &>(freelistAllocatorBuffer);
#else
alignas(IAllocator::DefaultAlignment) static uint8_t mallocAllocatorBuffer[sizeof(MallocAllocator)];
static MallocAllocator &mallocAllocator = reinterpret_cast<MallocAllocator &>(mallocAllocatorBuffer);
#endif

#ifdef WITH_IMGUI
alignas(IAllocator::DefaultAlignment) static uint8_t imguiAllocatorBuffer[sizeof(ProxyAllocator)];
static ProxyAllocator &imguiAllocator = reinterpret_cast<ProxyAllocator &>(imguiAllocatorBuffer);
#endif

#ifdef WITH_NUKLEAR
alignas(IAllocator::DefaultAlignment) static uint8_t nuklearAllocatorBuffer[sizeof(ProxyAllocator)];
static ProxyAllocator &nuklearAllocator = reinterpret_cast<ProxyAllocator &>(nuklearAllocatorBuffer);
#endif

#ifdef WITH_LUA
alignas(IAllocator::DefaultAlignment) static uint8_t luaAllocatorBuffer[sizeof(ProxyAllocator)];
static ProxyAllocator &luaAllocator = reinterpret_cast<ProxyAllocator &>(luaAllocatorBuffer);
#endif

AllocManager &theAllocManager()
{
	return allocManager;
}

IAllocator &theDefaultAllocator()
{
	return theAllocManager().defaultAllocator();
}

IAllocator &theStringAllocator()
{
	return theAllocManager().stringAllocator();
}

IAllocator &theImGuiAllocator()
{
#ifdef WITH_IMGUI
	return imguiAllocator;
#else
	return *mainAllocator;
#endif
}

IAllocator &theNuklearAllocator()
{
#ifdef WITH_NUKLEAR
	return nuklearAllocator;
#else
	return *mainAllocator;
#endif
}

IAllocator &theLuaAllocator()
{
#ifdef WITH_LUA
	return luaAllocator;
#else
	return *mainAllocator;
#endif
}

namespace {

#ifdef WITH_IMGUI
	void *imguiAllocate(size_t sz, void *userData)
	{
		return nctl::theImGuiAllocator().allocate(sz);
	}

	void imguiDeallocate(void *ptr, void *userData)
	{
		return nctl::theImGuiAllocator().deallocate(ptr);
	}
#endif

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AllocManager::AllocManager()
    : defaultAllocator_(nullptr), stringAllocator_(nullptr)
{
#ifdef USE_FREELIST
	new (&freelistAllocator) FreeListAllocator("Default", FreeListSize, freelistMemory);
	mainAllocator = &freelistAllocator;
#else
	new (&mallocAllocator) MallocAllocator();
	mainAllocator = &mallocAllocator;
#endif

	defaultAllocator_ = mainAllocator;
	stringAllocator_ = mainAllocator;

#ifdef WITH_IMGUI
	new (&imguiAllocator) ProxyAllocator("ImGui", *mainAllocator);
	ImGui::SetAllocatorFunctions(imguiAllocate, imguiDeallocate);
#endif
#ifdef WITH_NUKLEAR
	new (&nuklearAllocator) ProxyAllocator("Nuklear", *mainAllocator);
#endif
#ifdef WITH_LUA
	new (&luaAllocator) ProxyAllocator("Lua", *mainAllocator);
#endif
}

AllocManager::~AllocManager()
{
#ifdef WITH_LUA
	(&luaAllocator)->~ProxyAllocator();
#endif
#ifdef WITH_NUKLEAR
	(&nuklearAllocator)->~ProxyAllocator();
#endif
#ifdef WITH_IMGUI
	(&imguiAllocator)->~ProxyAllocator();
#endif

#ifdef USE_FREELIST
	(&freelistAllocator)->~FreeListAllocator();
#else
	(&mallocAllocator)->~MallocAllocator();
#endif
}

AllocManagerInitializer::AllocManagerInitializer()
{
	new (&allocManager) AllocManager();
}

AllocManagerInitializer::~AllocManagerInitializer()
{
	(&allocManager)->~AllocManager();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

IAllocator *AllocManager::setDefaultAllocator(IAllocator *allocator)
{
	IAllocator *previous = defaultAllocator_;
	defaultAllocator_ = (allocator != nullptr) ? allocator : mainAllocator;
	return previous;
}

IAllocator *AllocManager::setStringAllocator(IAllocator *allocator)
{
	IAllocator *previous = stringAllocator_;
	stringAllocator_ = (allocator != nullptr) ? allocator : mainAllocator;
	return previous;
}

}

#ifdef OVERRIDE_NEW
void *operator new(std::size_t count)
{
	if (count == 0)
		return nullptr;

	return nctl::theDefaultAllocator().allocate(count);
}

void operator delete(void *ptr) noexcept
{
	if (ptr != nullptr)
		nctl::theDefaultAllocator().deallocate(ptr);
}

void *operator new[](size_t count)
{
	if (count == 0)
		return nullptr;

	return nctl::theDefaultAllocator().allocate(count);
}

void operator delete[](void *ptr) noexcept
{
	if (ptr != nullptr)
		nctl::theDefaultAllocator().deallocate(ptr);
}
#endif
