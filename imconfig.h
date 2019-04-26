#pragma once

#include <ncine/Vector4.h>

#define IMGUI_API DLL_PUBLIC

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_DEMO_WINDOWS

#define IM_VEC2_CLASS_EXTRA \
	ImVec2(const ncine::Vector2f &f) \
	{ \
		x = f.x; \
		y = f.y; \
	} \
	operator ncine::Vector2f() const { return ncine::Vector2f(x, y); }

#define IM_VEC4_CLASS_EXTRA \
	ImVec4(const ncine::Vector4f &f) \
	{ \
		x = f.x; \
		y = f.y; \
		z = f.z; \
		w = f.w; \
	} \
	operator ncine::Vector4f() const { return ncine::Vector4f(x, y, z, w); }
