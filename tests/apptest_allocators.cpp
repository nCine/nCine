#include <ncine/imgui.h>

#include "apptest_allocators.h"
#include <nctl/LinearAllocator.h>
#include <nctl/StackAllocator.h>
#include <nctl/PoolAllocator.h>
#include <nctl/FreeListAllocator.h>
#include <nctl/Array.h>
#include <ncine/TimeStamp.h>
#include <ncine/Application.h>
#include "apptest_datapath.h"

namespace {

enum class AllocatorType
{
	LINEAR,
	STACK,
	POOL,
	FREELIST,
};

const char *typeStrings[4] = { "Linear", "Stack", "Pool", "Free List" };
const char *fitStrategyStrings[3] = { "First Fit", "Best Fit", "Worst Fit" };

struct Allocation
{
	nc::TimeStamp timestamp;
	void *ptr = nullptr;
	size_t size = 0;
	uint8_t alignment = nctl::IAllocator::DefaultAlignment;
};

struct Allocator
{
	AllocatorType type;
	nctl::UniquePtr<nctl::IAllocator> allocator;
	nctl::UniquePtr<uint8_t[]> buffer;
	nctl::Array<Allocation> allocations;
};

nctl::Array<Allocator> allocators;

int sanitizeAlignment(int value)
{
	int alignment = value;

	if (alignment > 128)
		alignment = 128;
	else if (alignment < 1)
		alignment = 1;
	// round to previous power of two
	while (alignment & alignment - 1)
		alignment = alignment & alignment - 1;

	return alignment;
}

bool mouseInsideRect(ImVec2 minRect, ImVec2 maxRect)
{
	const ImVec2 mousePos = ImGui::GetMousePos();
	if (mousePos.x >= minRect.x && mousePos.x <= maxRect.x &&
	    mousePos.y >= minRect.y && mousePos.y <= maxRect.y)
		return true;
	return false;
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);

	config.withAudio = false;
	config.withDebugOverlay = false;
	config.withThreads = false;
}

void MyEventHandler::onInit()
{
#ifdef __ANDROID__
	ImGuiIO &io = ImGui::GetIO();
	io.FontGlobalScale = 3.0f;
#endif
}

void MyEventHandler::onFrameStart()
{
	ImGui::SetNextWindowPos(ImVec2(50, 30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(1180, 660), ImGuiCond_FirstUseEver);
	ImGui::Begin("Memory Allocators");

	static int currentComboAllocatorEntry = 0;
	if (ImGui::TreeNode("Allocators"))
	{
		static int bufferSize = 1024;
		ImGui::SliderInt("Buffer Size", &bufferSize, 16, 4096);

		static int currentComboAllocatorType = 0;
		ImGui::Combo("Type", &currentComboAllocatorType, typeStrings, IM_ARRAYSIZE(typeStrings));
		AllocatorType allocatorType = static_cast<AllocatorType>(currentComboAllocatorType);

		static int elementSize = 128;
		static int elementAlignment = nctl::IAllocator::DefaultAlignment;
		if (allocatorType == AllocatorType::POOL)
		{
			ImGui::InputInt("Element Size", &elementSize);
			ImGui::SliderInt("Element Alignment", &elementAlignment, 1, 128);
			elementAlignment = sanitizeAlignment(elementAlignment);
		}

		if (ImGui::Button("Create##Allocator") && bufferSize > 0)
		{
			allocators.emplaceBack();
			allocators.back().type = allocatorType;
			nctl::UniquePtr<uint8_t[]> buffer = nctl::makeUnique<uint8_t[]>(bufferSize);
			switch (allocatorType)
			{
				case AllocatorType::LINEAR:
					allocators.back().allocator = nctl::makeUnique<nctl::LinearAllocator>(bufferSize, buffer.get());
					break;
				case AllocatorType::STACK:
					allocators.back().allocator = nctl::makeUnique<nctl::StackAllocator>(bufferSize, buffer.get());
					break;
				case AllocatorType::POOL:
					allocators.back().allocator = nctl::makeUnique<nctl::PoolAllocator>(elementSize, elementAlignment, bufferSize, buffer.get());
					break;
				case AllocatorType::FREELIST:
					allocators.back().allocator = nctl::makeUnique<nctl::FreeListAllocator>(bufferSize, buffer.get());
					break;
			}
			allocators.back().buffer = nctl::move(buffer);
		}
		ImGui::Separator();

		ImGui::Columns(8, "allocatorEntries", true);
		ImGui::Text("Entry");
		ImGui::NextColumn();
		ImGui::Text("Pointer");
		ImGui::NextColumn();
		ImGui::Text("Type");
		ImGui::NextColumn();
		ImGui::Text("Size");
		ImGui::NextColumn();
		ImGui::Text("Used");
		ImGui::NextColumn();
		ImGui::Text("Allocations");
		ImGui::NextColumn();
		ImGui::Text("Base Address");
		ImGui::NextColumn();
		ImGui::Text("Actions");
		ImGui::NextColumn();
		ImGui::Separator();
		for (unsigned int i = 0; i < allocators.size(); i++)
		{
			const AllocatorType type = allocators[i].type;
			nctl::IAllocator &alloc = *allocators[i].allocator;
			ImGui::Text("#%u", i);
			ImGui::NextColumn();
			ImGui::Text("0x%lx", uintptr_t(&alloc));
			ImGui::NextColumn();
			ImGui::Text("%s", typeStrings[static_cast<int>(allocators[i].type)]);
			ImGui::NextColumn();
			ImGui::Text("%lu", alloc.size());
			ImGui::NextColumn();
			const float usedFraction = alloc.usedMemory() / static_cast<float>(alloc.size());
			widgetName_.format("%lu / %lu", alloc.usedMemory(), alloc.size());
			ImGui::ProgressBar(usedFraction, ImVec2(0.0f, 0.0f), widgetName_.data());
			ImGui::NextColumn();
			ImGui::Text("%lu", alloc.numAllocations());
			ImGui::NextColumn();
			ImGui::Text("0x%lx", uintptr_t(alloc.base()));
			ImGui::NextColumn();
			if (type == AllocatorType::LINEAR && alloc.numAllocations() > 0)
			{
				widgetName_.format("Clear##0x%lx", &alloc);
				if (ImGui::Button(widgetName_.data()))
				{
					allocators[i].allocations.clear();
					static_cast<nctl::LinearAllocator &>(alloc).clear();
				}
				ImGui::SameLine();
			}
			else if (type == AllocatorType::FREELIST)
			{
				nctl::FreeListAllocator &flAlloc = static_cast<nctl::FreeListAllocator &>(alloc);
				if (flAlloc.defragOnDeallocation() == false)
				{
					widgetName_.format("Defrag##0x%lx", &alloc);
					if (ImGui::Button(widgetName_.data()))
					{
						nc::TimeStamp timestamp = nc::TimeStamp::now();
						flAlloc.defrag();
						LOGI_X("FreeList allocator derfagged in %f ms", timestamp.millisecondsSince());
					}
					if (alloc.numAllocations() == 0)
						ImGui::SameLine();
				}
			}

			if (alloc.numAllocations() == 0)
			{
				widgetName_.format("Delete##0x%lx", &alloc);
				if (ImGui::Button(widgetName_.data()))
				{
					allocators.removeAt(i);
					if (currentComboAllocatorEntry > allocators.size() - 1)
						currentComboAllocatorEntry = allocators.size() - 1;
				}
			}
			else if (type == AllocatorType::STACK || type == AllocatorType::POOL)
				ImGui::Text("Pending allocations");
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		ImGui::TreePop();
	}

	if (allocators.isEmpty() == false && ImGui::TreeNode("Memory Map"))
	{
		const AllocatorType selectedType = allocators[currentComboAllocatorEntry].type;
		nctl::IAllocator &selectedAlloc = *allocators[currentComboAllocatorEntry].allocator;
		nctl::Array<Allocation> &allocations = allocators[currentComboAllocatorEntry].allocations;

		const ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
		ImDrawList *drawList = ImGui::GetWindowDrawList();

		const float rectHeight = 4.0f * ImGui::GetFontSize();
		const float thickness = 2.0f * ImGui::GetIO().FontGlobalScale;
		const ImVec2 minRect(cursorScreenPos.x, cursorScreenPos.y);
		const ImVec2 maxRect(minRect.x + ImGui::GetContentRegionAvail().x, minRect.y + rectHeight);
		const float width = maxRect.x - minRect.x;
		const ImColor fillColor = (selectedType != AllocatorType::FREELIST) ? 0xffffffff : 0xffa9a9a9;
		drawList->AddRectFilled(minRect, maxRect, ImColor(fillColor), 0.0f, ImDrawCornerFlags_All);

		drawList->AddRect(minRect, maxRect, ImColor(0xffb1b9a8), 0.0f, ImDrawCornerFlags_All, 2.0f * thickness);
		drawList->AddRect(minRect, maxRect, ImColor(0xffceb89f), 0.0f, ImDrawCornerFlags_All, thickness);

		for (unsigned int i = 0; i < allocations.size(); i++)
		{
			const uintptr_t allocStart = uintptr_t(allocations[i].ptr) - uintptr_t(selectedAlloc.base());
			const uintptr_t allocEnd = allocStart + allocations[i].size;
			const float normStart = allocStart / static_cast<float>(selectedAlloc.size());
			const float normEnd = allocEnd / static_cast<float>(selectedAlloc.size());

			if (selectedType != AllocatorType::POOL && selectedType != AllocatorType::FREELIST)
			{
				const uintptr_t baseStart = (i == 0) ? 0 : uintptr_t(allocations[i - 1].ptr) + allocations[i - 1].size - uintptr_t(selectedAlloc.base());
				const float baseNormStart = baseStart / static_cast<float>(selectedAlloc.size());
				const ImVec2 baseMinRect(minRect.x + baseNormStart * width, minRect.y);
				const ImVec2 baseMaxRect(minRect.x + normStart * width, maxRect.y);
				drawList->AddRectFilled(baseMinRect, baseMaxRect, ImColor(0xffa9a9a9), 0.0f, ImDrawCornerFlags_All);
				drawList->AddRect(baseMinRect, baseMaxRect, ImColor(0xff595959), 0.0f, ImDrawCornerFlags_All, thickness);
			}

			const ImVec2 allocMinRect(minRect.x + normStart * width, minRect.y);
			const ImVec2 allocMaxRect(minRect.x + normEnd * width, maxRect.y);
			drawList->AddRectFilled(allocMinRect, allocMaxRect, ImColor(0xffbaf8f8), 0.0f, ImDrawCornerFlags_All);
			drawList->AddRect(allocMinRect, allocMaxRect, ImColor(0xffa90000), 0.0f, ImDrawCornerFlags_All, thickness);

			if (mouseInsideRect(allocMinRect, allocMaxRect))
			{
				const bool canDeallocate = (selectedType == AllocatorType::STACK && i == allocations.size() - 1) ||
				                           selectedType == AllocatorType::POOL || selectedType == AllocatorType::FREELIST;
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				widgetName_.format("#%u 0x%lx (+%lu)\n%lu bytes allocation", i, allocations[i].ptr, allocStart, allocations[i].size);
				if (canDeallocate)
					widgetName_.append("\nCTRL + left click to deallocate");
				ImGui::TextUnformatted(widgetName_.data());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
				if (canDeallocate && ImGui::GetIO().KeyCtrl && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					selectedAlloc.deallocate(allocations[i].ptr);
					allocations.removeAt(i);
				}
			}
		}

		if (selectedType == AllocatorType::POOL)
		{
			nctl::PoolAllocator &poolAlloc = static_cast<nctl::PoolAllocator &>(selectedAlloc);
			void **block = poolAlloc.freeList();
			unsigned int i = 0;
			while (block != nullptr)
			{
				const uintptr_t blockStart = uintptr_t(block) - uintptr_t(selectedAlloc.base());
				const uintptr_t blockEnd = blockStart + poolAlloc.elementSize();
				const float blockNormStart = blockStart / static_cast<float>(selectedAlloc.size());
				const float blockNormEnd = blockEnd / static_cast<float>(selectedAlloc.size());
				const ImVec2 blockMinRect(minRect.x + blockNormStart * width, minRect.y);
				const ImVec2 blockMaxRect(minRect.x + blockNormEnd * width, maxRect.y);
				drawList->AddRectFilled(blockMinRect, blockMaxRect, ImColor(0xffdfdfdf), 0.0f, ImDrawCornerFlags_All);
				drawList->AddRect(blockMinRect, blockMaxRect, ImColor(0xff792979), 0.0f, ImDrawCornerFlags_All, 2.0f);

				if (mouseInsideRect(blockMinRect, blockMaxRect))
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					widgetName_.format("#%u 0x%lx (+%lu)\n%lu bytes free block", i, block, blockStart, poolAlloc.elementSize());
					ImGui::TextUnformatted(widgetName_.data());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				i++;
				block = reinterpret_cast<void **>(*block);
			}
		}
		else if (selectedType == AllocatorType::FREELIST)
		{
			nctl::FreeListAllocator &flAlloc = static_cast<nctl::FreeListAllocator &>(selectedAlloc);
			const nctl::FreeListAllocator::Block *block = flAlloc.freeBlock();
			unsigned int i = 0;
			while (block != nullptr)
			{
				const uintptr_t blockStart = uintptr_t(block) - uintptr_t(selectedAlloc.base());
				const uintptr_t blockEnd = blockStart + block->size;
				const float blockNormStart = blockStart / static_cast<float>(selectedAlloc.size());
				const float blockNormEnd = blockEnd / static_cast<float>(selectedAlloc.size());
				const ImVec2 blockMinRect(minRect.x + blockNormStart * width, minRect.y);
				const ImVec2 blockMaxRect(minRect.x + blockNormEnd * width, maxRect.y);
				drawList->AddRectFilled(blockMinRect, blockMaxRect, ImColor(0xffdfdfdf), 0.0f, ImDrawCornerFlags_All);
				drawList->AddRect(blockMinRect, blockMaxRect, ImColor(0xff792979), 0.0f, ImDrawCornerFlags_All, 2.0f);

				if (mouseInsideRect(blockMinRect, blockMaxRect))
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					widgetName_.format("#%u 0x%lx (+%lu)\n%lu bytes free block", i, block, blockStart, block->size);
					ImGui::TextUnformatted(widgetName_.data());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				i++;
				block = block->next;
			}
		}
		ImGui::SetCursorScreenPos(maxRect);
		ImGui::Spacing();

		if (selectedType == AllocatorType::FREELIST)
		{
			nctl::FreeListAllocator &flAlloc = static_cast<nctl::FreeListAllocator &>(selectedAlloc);
			const nctl::FreeListAllocator::Block *block = flAlloc.freeBlock();

			ImGui::Columns(3, "freelistEntries", true);
			ImGui::Text("Entry");
			ImGui::NextColumn();
			ImGui::Text("Pointer");
			ImGui::NextColumn();
			ImGui::Text("Size");
			ImGui::NextColumn();
			ImGui::Separator();
			unsigned int i = 0;
			while (block != nullptr)
			{
				ImGui::Text("#%u", i);
				ImGui::NextColumn();
				ImGui::Text("0x%lx (+%lu)", uintptr_t(block), uintptr_t(block) - uintptr_t(flAlloc.base()));
				ImGui::NextColumn();
				ImGui::Text("%lu", block->size);
				ImGui::NextColumn();

				i++;
				block = block->next;
			}
			ImGui::Columns(1);
		}

		ImGui::TreePop();
	}

	if (allocators.isEmpty() == false && ImGui::TreeNode("Allocations"))
	{
		comboAllocators_.clear();
		for (unsigned int i = 0; i < allocators.size(); i++)
		{
			const nctl::IAllocator &alloc = *allocators[i].allocator;
			comboAllocators_.formatAppend("#%u %s (%lu / %lu)", i, typeStrings[static_cast<int>(allocators[i].type)],
			                              alloc.usedMemory(), alloc.size());
			comboAllocators_.setLength(comboAllocators_.length() + 1);
		}
		comboAllocators_.setLength(comboAllocators_.length() + 1);
		// Append a second '\0' to signal the end of the combo item list
		comboAllocators_[comboAllocators_.length() - 1] = '\0';
		ImGui::Combo("Allocator", &currentComboAllocatorEntry, comboAllocators_.data(), allocators.size());

		const AllocatorType selectedType = allocators[currentComboAllocatorEntry].type;
		nctl::IAllocator &selectedAlloc = *allocators[currentComboAllocatorEntry].allocator;
		nctl::Array<Allocation> &allocations = allocators[currentComboAllocatorEntry].allocations;

		static int size = 128;
		static int alignment = nctl::IAllocator::DefaultAlignment;
		if (selectedType != AllocatorType::POOL)
		{
			ImGui::SliderInt("Size", &size, 1, 4096);
			ImGui::SliderInt("Alignment", &alignment, 1, 128);
			alignment = sanitizeAlignment(alignment);
			ImGui::SameLine();
			if (ImGui::Button("Reset##Alignment"))
				alignment = nctl::IAllocator::DefaultAlignment;
		}
		else
		{
			const nctl::PoolAllocator &poolAllocator = static_cast<nctl::PoolAllocator &>(selectedAlloc);
			size = poolAllocator.elementSize();
			alignment = poolAllocator.elementAlignment();
			ImGui::Text("Size: %u", size);
			ImGui::Text("Alignment: %u", alignment);
		}

		if (ImGui::Button("Allocate") && size > 0 && (alignment & (alignment - 1)) == 0)
		{
			void *ptr = selectedAlloc.allocate(size, alignment);
			if (ptr != nullptr)
			{
				allocations.emplaceBack();
				allocations.back().timestamp = nc::TimeStamp::now();
				allocations.back().ptr = ptr;
				allocations.back().size = size;
				allocations.back().alignment = alignment;
			}
			else
				LOGW_X("Could not allocate %lu bytes", size);
		}
		if (selectedType == AllocatorType::FREELIST)
		{
			nctl::FreeListAllocator &flAlloc = static_cast<nctl::FreeListAllocator &>(selectedAlloc);

			ImGui::SameLine();
			static int currentComboFitStrategy = 0;
			currentComboFitStrategy = static_cast<int>(flAlloc.fitStrategy());
			ImGui::PushItemWidth(100.0f * ImGui::GetIO().FontGlobalScale);
			ImGui::Combo("Fit Strategy", &currentComboFitStrategy, fitStrategyStrings, IM_ARRAYSIZE(fitStrategyStrings));
			ImGui::PopItemWidth();
			nctl::FreeListAllocator::FitStrategy fitStrategy = static_cast<nctl::FreeListAllocator::FitStrategy>(currentComboFitStrategy);
			flAlloc.setFitStrategy(fitStrategy);

			ImGui::SameLine();
			bool defragOnDeallocation = flAlloc.defragOnDeallocation();
			ImGui::Checkbox("Defrag on Deallocation", &defragOnDeallocation);
			flAlloc.setDefragOnDeallocation(defragOnDeallocation);

			ImGui::SameLine();
			bool copyOnReallocation = flAlloc.copyOnReallocation();
			ImGui::Checkbox("Copy on Reallocation", &copyOnReallocation);
			flAlloc.setCopyOnReallocation(copyOnReallocation);
		}
		ImGui::Separator();

		ImGui::Columns(7, "allocationEntries", true);
		ImGui::Text("Entry");
		ImGui::NextColumn();
		ImGui::Text("Timestamp");
		ImGui::NextColumn();
		ImGui::Text("Address");
		ImGui::NextColumn();
		ImGui::Text("Size");
		ImGui::NextColumn();
		ImGui::Text("Alignment");
		ImGui::NextColumn();
		ImGui::Text("Realloc");
		ImGui::NextColumn();
		ImGui::Text("Free");
		ImGui::NextColumn();
		ImGui::Separator();
		for (unsigned int i = 0; i < allocations.size(); i++)
		{
			const Allocation &e = allocations[i];

			ImGui::Text("#%u", i);
			ImGui::NextColumn();
			ImGui::Text("%fs", e.timestamp.seconds());
			ImGui::NextColumn();
			const int baseOffset = uintptr_t(e.ptr) - uintptr_t(selectedAlloc.base());
			ImGui::Text("0x%lx (%+d)", uintptr_t(e.ptr), baseOffset);
			ImGui::NextColumn();
			ImGui::Text("%lu", e.size);
			ImGui::NextColumn();
			ImGui::Text("%u", e.alignment);
			ImGui::NextColumn();

			if (selectedType == AllocatorType::LINEAR)
				ImGui::Text("Linear cannot reallocate");
			else if (selectedType == AllocatorType::STACK && i != allocations.size() - 1)
				ImGui::Text("Stack can only reallocate last one");
			else if (selectedType == AllocatorType::POOL)
				ImGui::Text("Pool cannot reallocate");
			else
			{
				widgetName_.format("Realloc##0x%lx", e.ptr);
				if (ImGui::Button(widgetName_.data()))
				{
					void *ptr = selectedAlloc.reallocate(e.ptr, size);
					if (ptr != nullptr)
					{
						allocations[i].ptr = ptr;
						allocations[i].size = size;
					}
					else
						LOGW_X("Could not reallocate 0x%lx from %lu to %lu bytes", allocations[i].ptr, allocations[i].size, size);
				}
			}
			ImGui::NextColumn();

			if (selectedType == AllocatorType::LINEAR)
				ImGui::Text("Linear cannot free");
			else if (selectedType == AllocatorType::STACK && i != allocations.size() - 1)
				ImGui::Text("Stack can only free last one");
			else
			{
				widgetName_.format("Free##0x%lx", e.ptr);
				if (ImGui::Button(widgetName_.data()))
				{
					selectedAlloc.deallocate(e.ptr);
					allocations.removeAt(i);
				}
			}
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		ImGui::TreePop();
	}

	ImGui::End();
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
