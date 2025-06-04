#include <new>
#include <ncine/allocators_config.h>
#include <nctl/AllocManager.h>
#include <nctl/MallocAllocator.h>
#include <nctl/FreeListAllocator.h>
#include <nctl/ProxyAllocator.h>

#ifdef WITH_GLFW
	#include <GLFW/glfw3.h>
	#define GLFW_VERSION_COMBINED (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 + GLFW_VERSION_REVISION)
#endif

#ifdef WITH_SDL
	#include <SDL2/SDL_stdinc.h>
#endif

#ifdef WITH_IMGUI
	#include "imgui.h"
#endif

#ifdef _MSC_VER
	#pragma warning(disable: 4075)
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

#if defined(WITH_GLFW) && GLFW_VERSION_COMBINED >= 3400
alignas(IAllocator::DefaultAlignment) static uint8_t glfwAllocatorBuffer[sizeof(ProxyAllocator)];
static ProxyAllocator &glfwAllocator = reinterpret_cast<ProxyAllocator &>(glfwAllocatorBuffer);
#endif

#if defined(WITH_SDL)
alignas(IAllocator::DefaultAlignment) static uint8_t sdl2AllocatorBuffer[sizeof(ProxyAllocator)];
static ProxyAllocator &sdl2Allocator = reinterpret_cast<ProxyAllocator &>(sdl2AllocatorBuffer);
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

IAllocator &theGlfwAllocator()
{
#if defined(WITH_GLFW) && GLFW_VERSION_COMBINED >= 3400
	return glfwAllocator;
#else
	return *mainAllocator;
#endif
}

IAllocator &theSdl2Allocator()
{
#if defined(WITH_SDL)
	return sdl2Allocator;
#else
	return *mainAllocator;
#endif
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

#if defined(WITH_GLFW) && GLFW_VERSION_COMBINED >= 3400
	void *glfwAllocate(size_t size, void *user)
	{
		return nctl::theGlfwAllocator().allocate(size);
	}

	void *glfwReallocate(void *block, size_t size, void *user)
	{
		return nctl::theGlfwAllocator().reallocate(block, size);
	}

	void glfwDeallocate(void *block, void *user)
	{
		return nctl::theGlfwAllocator().deallocate(block);
	}
#endif

#if defined(WITH_SDL)
	void *sdl2Allocate(size_t size)
	{
		return nctl::theSdl2Allocator().allocate(size);
	}

	void *sdl2ClearAllocate(size_t nmemb, size_t size)
	{
		void *ptr = nctl::theSdl2Allocator().allocate(nmemb * size);
		memset(ptr, 0, nmemb * size);
		return ptr;
	}

	void *sdl2Reallocate(void *block, size_t size)
	{
		return nctl::theSdl2Allocator().reallocate(block, size);
	}

	void sdl2Deallocate(void *block)
	{
		return nctl::theSdl2Allocator().deallocate(block);
	}
#endif

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

#if defined(WITH_GLFW) && GLFW_VERSION_COMBINED >= 3400
	new (&glfwAllocator) ProxyAllocator("GLFW", *mainAllocator);
	GLFWallocator allocator;
	allocator.allocate = glfwAllocate;
	allocator.reallocate = glfwReallocate;
	allocator.deallocate = glfwDeallocate;
	allocator.user = nullptr;

	glfwInitAllocator(&allocator);
#endif
#if defined(WITH_SDL)
	new (&sdl2Allocator) ProxyAllocator("SDL2", *mainAllocator);
	SDL_SetMemoryFunctions(sdl2Allocate, sdl2ClearAllocate, sdl2Reallocate, sdl2Deallocate);
#endif
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
#if defined(WITH_GLFW) && GLFW_VERSION_COMBINED >= 3400
	(&glfwAllocator)->~ProxyAllocator();
#endif
#if defined(WITH_SDL)
	(&sdl2Allocator)->~ProxyAllocator();
#endif
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
