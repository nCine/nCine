if(NCINE_WITH_VULKAN)
	target_compile_definitions(ncine PRIVATE "WITH_GRAIL")
	target_compile_definitions(ncine PRIVATE "WITH_VULKAN")
	target_include_directories(ncine PRIVATE ${Vulkan_INCLUDE_DIR})
	target_sources(ncine PRIVATE ${VOLK_C_SOURCE})

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/grail/config.h
		${NCINE_ROOT}/src/include/grail/formats.h
		${NCINE_ROOT}/src/include/grail/types.h
		${NCINE_ROOT}/src/include/grail/Handle.h
		${NCINE_ROOT}/src/include/grail/handle_types.h
		${NCINE_ROOT}/src/include/grail/Device.h
		${NCINE_ROOT}/src/include/grail/Swapchain.h
		${NCINE_ROOT}/src/include/grail/BindGroupLayout.h
		${NCINE_ROOT}/src/include/grail/BindGroup.h
		${NCINE_ROOT}/src/include/grail/pipeline.h
		${NCINE_ROOT}/src/include/grail/GraphicsPipeline.h
		${NCINE_ROOT}/src/include/grail/ComputePipeline.h
		${NCINE_ROOT}/src/include/grail/Buffer.h
		${NCINE_ROOT}/src/include/grail/Texture.h
		${NCINE_ROOT}/src/include/grail/TextureView.h
		${NCINE_ROOT}/src/include/grail/CommandBuffer.h
		${NCINE_ROOT}/src/include/grail/RenderPass.h

		${NCINE_ROOT}/src/include/grail/private/GenerationalPool.h
		${NCINE_ROOT}/src/include/grail/private/HandlePool.h
		${NCINE_ROOT}/src/include/grail/private/hash_functions.h
		${NCINE_ROOT}/src/include/grail/private/hash_types.h
		${NCINE_ROOT}/src/include/grail/private/Device_frontend.h

		${NCINE_ROOT}/src/include/grail/vulkan/vlk_utils.h
		${NCINE_ROOT}/src/include/grail/vulkan/Device_backend.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/graphics/grail/hash_functions.cpp
		${NCINE_ROOT}/src/graphics/grail/GenerationalPool.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_frontend.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_bindgroup_layout_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_bindgroup_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_graphics_pipeline_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_compute_pipeline_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_buffer_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_texture_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_texture_view_front.cpp
		${NCINE_ROOT}/src/graphics/grail/Device_command_buffer_front.cpp

		${NCINE_ROOT}/src/graphics/grail/vulkan/vlk_utils.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_instance.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_surface.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_device.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Swapchain.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_depth_format.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_synchronization.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_frame_context.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_bindgroup_layout.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_bindgroup.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_pipeline.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_graphics_pipeline.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_compute_pipeline.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_buffer.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_texture.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_texture_view.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/Device_command_buffer.cpp
		${NCINE_ROOT}/src/graphics/grail/vulkan/CommandBuffer.cpp
	)
endif()
