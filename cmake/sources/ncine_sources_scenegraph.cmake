if(NCINE_WITH_SCENEGRAPH)
	target_compile_definitions(ncine PRIVATE "WITH_SCENEGRAPH")

	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/DrawableNode.h
		${NCINE_ROOT}/include/ncine/Font.h
		${NCINE_ROOT}/include/ncine/Texture.h
		${NCINE_ROOT}/include/ncine/Shader.h
		${NCINE_ROOT}/include/ncine/ShaderState.h
		${NCINE_ROOT}/include/ncine/SceneNode.h
		${NCINE_ROOT}/include/ncine/BaseSprite.h
		${NCINE_ROOT}/include/ncine/Sprite.h
		${NCINE_ROOT}/include/ncine/MeshSprite.h
		${NCINE_ROOT}/include/ncine/Particle.h
		${NCINE_ROOT}/include/ncine/ParticleAffectors.h
		${NCINE_ROOT}/include/ncine/ParticleSystem.h
		${NCINE_ROOT}/include/ncine/ParticleInitializer.h
		${NCINE_ROOT}/include/ncine/TextNode.h
		${NCINE_ROOT}/include/ncine/RectAnimation.h
		${NCINE_ROOT}/include/ncine/AnimatedSprite.h
		${NCINE_ROOT}/include/ncine/Viewport.h
		${NCINE_ROOT}/include/ncine/Camera.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/FntParser.h
		${NCINE_ROOT}/src/include/FontGlyph.h
		${NCINE_ROOT}/src/include/RenderCommand.h
		${NCINE_ROOT}/src/include/RenderQueue.h
		${NCINE_ROOT}/src/include/Material.h
		${NCINE_ROOT}/src/include/Geometry.h
		${NCINE_ROOT}/src/include/RenderBatcher.h
		${NCINE_ROOT}/src/include/RenderCommandPool.h
		${NCINE_ROOT}/src/include/ScreenViewport.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/Font.cpp
		${NCINE_ROOT}/src/FntParser.cpp
		${NCINE_ROOT}/src/FontGlyph.cpp
		${NCINE_ROOT}/src/graphics/RenderCommand.cpp
		${NCINE_ROOT}/src/graphics/RenderQueue.cpp
		${NCINE_ROOT}/src/graphics/Material.cpp
		${NCINE_ROOT}/src/graphics/Geometry.cpp
		${NCINE_ROOT}/src/graphics/Texture.cpp
		${NCINE_ROOT}/src/graphics/Shader.cpp
		${NCINE_ROOT}/src/graphics/ShaderState.cpp
		${NCINE_ROOT}/src/graphics/DrawableNode.cpp
		${NCINE_ROOT}/src/graphics/SceneNode.cpp
		${NCINE_ROOT}/src/graphics/BaseSprite.cpp
		${NCINE_ROOT}/src/graphics/Sprite.cpp
		${NCINE_ROOT}/src/graphics/MeshSprite.cpp
		${NCINE_ROOT}/src/graphics/Particle.cpp
		${NCINE_ROOT}/src/graphics/ParticleAffectors.cpp
		${NCINE_ROOT}/src/graphics/ParticleSystem.cpp
		${NCINE_ROOT}/src/graphics/ParticleInitializer.cpp
		${NCINE_ROOT}/src/graphics/TextNode.cpp
		${NCINE_ROOT}/src/graphics/RectAnimation.cpp
		${NCINE_ROOT}/src/graphics/AnimatedSprite.cpp
		${NCINE_ROOT}/src/graphics/RenderBatcher.cpp
		${NCINE_ROOT}/src/graphics/RenderCommandPool.cpp
		${NCINE_ROOT}/src/graphics/Viewport.cpp
		${NCINE_ROOT}/src/graphics/ScreenViewport.cpp
		${NCINE_ROOT}/src/graphics/Camera.cpp
	)
endif()
