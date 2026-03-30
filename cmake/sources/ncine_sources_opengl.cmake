if(OPENGL_FOUND)
	target_link_libraries(ncine PRIVATE OpenGL::GL)
endif()

list(APPEND PRIVATE_HEADERS
	${NCINE_ROOT}/src/include/GfxCapabilities.h
	${NCINE_ROOT}/src/include/RenderResources.h
	${NCINE_ROOT}/src/include/TextureFormat.h
	${NCINE_ROOT}/src/include/ITextureLoader.h
	${NCINE_ROOT}/src/include/TextureLoaderRaw.h
	${NCINE_ROOT}/src/include/TextureLoaderDds.h
	${NCINE_ROOT}/src/include/TextureLoaderPvr.h
	${NCINE_ROOT}/src/include/TextureLoaderKtx.h
	${NCINE_ROOT}/src/include/GLHashMap.h
	${NCINE_ROOT}/src/include/GLBufferObject.h
	${NCINE_ROOT}/src/include/GLBufferObject.h
	${NCINE_ROOT}/src/include/GLFramebufferObject.h
	${NCINE_ROOT}/src/include/GLRenderbuffer.h
	${NCINE_ROOT}/src/include/GLShader.h
	${NCINE_ROOT}/src/include/GLShaderProgram.h
	${NCINE_ROOT}/src/include/GLShaderUniforms.h
	${NCINE_ROOT}/src/include/GLUniform.h
	${NCINE_ROOT}/src/include/GLUniformCache.h
	${NCINE_ROOT}/src/include/GLAttribute.h
	${NCINE_ROOT}/src/include/GLTexture.h
	${NCINE_ROOT}/src/include/GLVertexArrayObject.h
	${NCINE_ROOT}/src/include/GLUniformBlock.h
	${NCINE_ROOT}/src/include/GLUniformBlockCache.h
	${NCINE_ROOT}/src/include/GLShaderUniformBlocks.h
	${NCINE_ROOT}/src/include/GLScissorTest.h
	${NCINE_ROOT}/src/include/GLDepthTest.h
	${NCINE_ROOT}/src/include/GLBlending.h
	${NCINE_ROOT}/src/include/GLCullFace.h
	${NCINE_ROOT}/src/include/GLClearColor.h
	${NCINE_ROOT}/src/include/GLViewport.h
	${NCINE_ROOT}/src/include/RenderBuffersManager.h
	${NCINE_ROOT}/src/include/GLDebug.h
	${NCINE_ROOT}/src/include/RenderStatistics.h
	${NCINE_ROOT}/src/include/GLVertexFormat.h
	${NCINE_ROOT}/src/include/RenderVaoPool.h
	${NCINE_ROOT}/src/include/BinaryShaderCache.h
)

list(APPEND SOURCES
	${NCINE_ROOT}/src/graphics/GfxCapabilities.cpp
	${NCINE_ROOT}/src/graphics/RenderResources.cpp
	${NCINE_ROOT}/src/graphics/TextureFormat.cpp
	${NCINE_ROOT}/src/graphics/ITextureLoader.cpp
	${NCINE_ROOT}/src/graphics/TextureLoaderRaw.cpp
	${NCINE_ROOT}/src/graphics/TextureLoaderDds.cpp
	${NCINE_ROOT}/src/graphics/TextureLoaderPvr.cpp
	${NCINE_ROOT}/src/graphics/TextureLoaderKtx.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLBufferObject.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLFramebufferObject.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLRenderbuffer.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLShader.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLShaderProgram.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLShaderUniforms.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLUniform.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLUniformCache.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLAttribute.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLTexture.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLVertexArrayObject.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLUniformBlock.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLUniformBlockCache.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLShaderUniformBlocks.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLScissorTest.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLDepthTest.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLBlending.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLCullFace.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLClearColor.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLViewport.cpp
	${NCINE_ROOT}/src/graphics/RenderBuffersManager.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLDebug.cpp
	${NCINE_ROOT}/src/graphics/RenderStatistics.cpp
	${NCINE_ROOT}/src/graphics/opengl/GLVertexFormat.cpp
	${NCINE_ROOT}/src/graphics/RenderVaoPool.cpp
	${NCINE_ROOT}/src/graphics/BinaryShaderCache.cpp
)

if(ANGLE_FOUND OR OPENGLES2_FOUND)
	target_compile_definitions(ncine PRIVATE "WITH_OPENGLES")
	target_link_libraries(ncine PRIVATE EGL::EGL OpenGLES2::GLES2)

	if(ANGLE_FOUND)
		message(STATUS "ANGLE has been found")
		target_compile_definitions(ncine PRIVATE "WITH_ANGLE")
	endif()

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/TextureLoaderPkm.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/TextureLoaderPkm.cpp)
endif()

if(GLEW_FOUND)
	if (NOT WIN32)
		message(STATUS "GLEW has been found")
	endif()
	target_compile_definitions(ncine PRIVATE "WITH_GLEW")
	target_link_libraries(ncine PRIVATE GLEW::GLEW)
endif()
