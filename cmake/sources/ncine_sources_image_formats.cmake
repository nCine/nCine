if(PNG_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_PNG")
	target_link_libraries(ncine PRIVATE PNG::PNG)

	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/ImageSaverPng.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImageLoaderPng.h)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/graphics/ImageLoaderPng.cpp
		${NCINE_ROOT}/src/graphics/ImageSaverPng.cpp)
endif()
if(WEBP_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_WEBP")
	target_link_libraries(ncine PRIVATE WebP::WebP)

	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/ImageSaverWebP.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImageLoaderWebP.h)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/graphics/ImageLoaderWebP.cpp
		${NCINE_ROOT}/src/graphics/ImageSaverWebP.cpp)
endif()
if(NCINE_WITH_QOI)
	target_compile_definitions(ncine PRIVATE "WITH_QOI")

	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/ImageSaverQoi.h)
	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImageLoaderQoi.h)
	list(APPEND SOURCES
		${QOI_SOURCE_DIR}/qoi.h
		${NCINE_ROOT}/src/graphics/ImageLoaderQoi.cpp
		${NCINE_ROOT}/src/graphics/ImageSaverQoi.cpp)
endif()
