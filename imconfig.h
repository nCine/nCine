#pragma once

#include <ncine/Vector4.h>
#include <ncine/Colorf.h>

#define IMGUI_API DLL_PUBLIC

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_OBSOLETE_KEYIO
#define IMGUI_DISABLE_DEMO_WINDOWS
#define IMGUI_DISABLE_DEBUG_TOOLS // (this was called IMGUI_DISABLE_METRICS_WINDOW before 1.88).

#define IM_VEC2_CLASS_EXTRA \
	constexpr ImVec2(const ncine::Vector2f &f) : x(f.x), y(f.y) {} \
	operator ncine::Vector2f() const { return ncine::Vector2f(x, y); }

#define IM_VEC4_CLASS_EXTRA \
	constexpr ImVec4(const ncine::Vector4f &f) : x(f.x), y(f.y), z(f.z), w(f.w) {} \
	operator ncine::Vector4f() const { return ncine::Vector4f(x, y, z, w); } \
	\
	ImVec4(const ncine::Colorf &c) : x(c.r()), y(c.g()), z(c.b()), w(c.a()) {} \
	operator ncine::Colorf() const { return ncine::Colorf(x, y, z, w); }
