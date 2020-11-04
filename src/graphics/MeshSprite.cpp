#include <cstring> // for memcpy()
#include "MeshSprite.h"
#include "RenderCommand.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \note The initial layer value for a mesh sprite is `DrawableNode::SCENE_LAYER` */
MeshSprite::MeshSprite(SceneNode *parent, Texture *texture)
    : MeshSprite(parent, texture, 0.0f, 0.0f)
{
}

/*! \note The initial layer value for a mesh sprite is `DrawableNode::SCENE_LAYER` */
MeshSprite::MeshSprite(Texture *texture)
    : MeshSprite(nullptr, texture, 0.0f, 0.0f)
{
}

/*! \note The initial layer value for a mesh sprite is `DrawableNode::SCENE_LAYER` */
MeshSprite::MeshSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : BaseSprite(parent, texture, xx, yy),
      interleavedVertices_(16), vertexDataPointer_(nullptr), numVertices_(0),
      indices_(16), indexDataPointer_(nullptr), numIndices_(0)
{
	ASSERT(texture);

	ZoneScoped;
	ZoneText(texture->name().data(), texture->name().length());
	type_ = ObjectType::MESH_SPRITE;
	setLayer(DrawableNode::LayerBase::SCENE);
	renderCommand_->setType(RenderCommand::CommandTypes::MESH_SPRITE);
	const Material::ShaderProgramType shaderProgramType = texture_->numChannels() >= 3
	                                                          ? Material::ShaderProgramType::MESH_SPRITE
	                                                          : Material::ShaderProgramType::MESH_SPRITE_GRAY;
	renderCommand_->material().setShaderProgramType(shaderProgramType);
	spriteBlock_ = renderCommand_->material().uniformBlock("MeshSpriteBlock");
	renderCommand_->geometry().setPrimitiveType(GL_TRIANGLE_STRIP);
	renderCommand_->geometry().setNumElementsPerVertex(sizeof(Vertex) / sizeof(float));
	renderCommand_->geometry().setHostVertexPointer(reinterpret_cast<const float *>(vertexDataPointer_));

	setTexRect(Recti(0, 0, texture_->width(), texture_->height()));
}

/*! \note The initial layer value for a mesh sprite is `DrawableNode::SCENE_LAYER` */
MeshSprite::MeshSprite(SceneNode *parent, Texture *texture, const Vector2f &position)
    : MeshSprite(parent, texture, position.x, position.y)
{
}

/*! \note The initial layer value for a mesh sprite is `DrawableNode::SCENE_LAYER` */
MeshSprite::MeshSprite(Texture *texture, float xx, float yy)
    : MeshSprite(nullptr, texture, xx, yy)
{
}

/*! \note The initial layer value for a mesh sprite is `DrawableNode::SCENE_LAYER` */
MeshSprite::MeshSprite(Texture *texture, const Vector2f &position)
    : MeshSprite(nullptr, texture, position.x, position.y)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void MeshSprite::copyVertices(unsigned int numVertices, const Vertex *vertices)
{
	interleavedVertices_.setSize(0);
	interleavedVertices_.setSize(numVertices);
	memcpy(interleavedVertices_.data(), vertices, numVertices * sizeof(Vertex));

	vertexDataPointer_ = interleavedVertices_.data();
	numVertices_ = numVertices;
	renderCommand_->geometry().setNumVertices(numVertices);
	renderCommand_->geometry().setHostVertexPointer(reinterpret_cast<const float *>(vertexDataPointer_));
}

void MeshSprite::copyVertices(const MeshSprite &meshSprite)
{
	copyVertices(meshSprite.numVertices_, meshSprite.vertexDataPointer_);
	width_ = meshSprite.width_;
	height_ = meshSprite.height_;
	texRect_ = meshSprite.texRect_;
}

void MeshSprite::setVertices(unsigned int numVertices, const Vertex *vertices)
{
	interleavedVertices_.setSize(0);

	vertexDataPointer_ = vertices;
	numVertices_ = numVertices;
	renderCommand_->geometry().setNumVertices(numVertices);
	renderCommand_->geometry().setHostVertexPointer(reinterpret_cast<const float *>(vertexDataPointer_));
}

void MeshSprite::setVertices(const MeshSprite &meshSprite)
{
	setVertices(meshSprite.numVertices_, meshSprite.vertexDataPointer_);
	width_ = meshSprite.width_;
	height_ = meshSprite.height_;
}

void MeshSprite::createVerticesFromTexels(unsigned int numVertices, const Vector2f *points, TextureCutMode cutMode)
{
	FATAL_ASSERT(numVertices >= 3);

	interleavedVertices_.setSize(0);
	interleavedVertices_.setSize(numVertices);
	Vector2f min(0.0f, 0.0f);

	if (cutMode == TextureCutMode::CROP)
	{
		min = points[0];
		Vector2f max(min);
		for (unsigned int i = 1; i < numVertices; i++)
		{
			if (points[i].x > max.x)
				max.x = points[i].x;
			else if (points[i].x < min.x)
				min.x = points[i].x;

			if (points[i].y > max.y)
				max.y = points[i].y;
			else if (points[i].y < min.y)
				min.y = points[i].y;
		}

		width_ = max.x - min.x;
		height_ = max.y - min.y;
	}
	else
	{
		width_ = static_cast<float>(texRect_.w);
		height_ = static_cast<float>(texRect_.h);
	}

	const float halfWidth = width_ * 0.5f;
	const float halfHeight = height_ * 0.5f;

	for (unsigned int i = 0; i < numVertices; i++)
	{
		Vertex &v = interleavedVertices_[i];
		v.x = (points[i].x - min.x - halfWidth) / width_; // from -0.5 to 0.5
		v.y = (points[i].y - min.y - halfHeight) / height_; // from -0.5 to 0.5
		v.u = points[i].x / (texRect_.w - texRect_.x);
		v.v = (texRect_.h - points[i].y) / (texRect_.h - texRect_.y); // flipped
	}

	vertexDataPointer_ = interleavedVertices_.data();
	numVertices_ = numVertices;
	renderCommand_->geometry().setNumVertices(numVertices);
	renderCommand_->geometry().setHostVertexPointer(reinterpret_cast<const float *>(vertexDataPointer_));
}

void MeshSprite::createVerticesFromTexels(unsigned int numVertices, const Vector2f *points)
{
	createVerticesFromTexels(numVertices, points, TextureCutMode::RESIZE);
}

void MeshSprite::copyIndices(unsigned int numIndices, const unsigned short *indices)
{
	indices_.setSize(0);
	indices_.setSize(numIndices);
	memcpy(indices_.data(), indices, numIndices * sizeof(unsigned short));

	indexDataPointer_ = indices_.data();
	numIndices_ = numIndices;
	renderCommand_->geometry().setNumIndices(numIndices_);
	renderCommand_->geometry().setHostIndexPointer(indexDataPointer_);
}

void MeshSprite::copyIndices(const MeshSprite &meshSprite)
{
	copyIndices(meshSprite.numIndices_, meshSprite.indexDataPointer_);
}

void MeshSprite::setIndices(unsigned int numIndices, const unsigned short *indices)
{
	indices_.setSize(0);

	indexDataPointer_ = indices;
	numIndices_ = numIndices;
	renderCommand_->geometry().setNumIndices(numIndices_);
	renderCommand_->geometry().setHostIndexPointer(indexDataPointer_);
}

void MeshSprite::setIndices(const MeshSprite &meshSprite)
{
	setIndices(meshSprite.numIndices_, meshSprite.indexDataPointer_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void MeshSprite::textureHasChanged(Texture *newTexture)
{
	if (renderCommand_->material().shaderProgramType() != Material::ShaderProgramType::CUSTOM)
	{
		const Material::ShaderProgramType shaderProgramType = newTexture->numChannels() >= 3
		                                                          ? Material::ShaderProgramType::MESH_SPRITE
		                                                          : Material::ShaderProgramType::MESH_SPRITE_GRAY;
		renderCommand_->material().setShaderProgramType(shaderProgramType);
	}

	if (texture_ != newTexture)
	{
		Recti texRect = texRect_;
		texRect.x = (texRect.x / float(texture_->width())) * float(newTexture->width());
		texRect.y = (texRect.y / float(texture_->height())) * float(newTexture->width());
		texRect.w = (texRect.w / float(texture_->width())) * float(newTexture->width());
		texRect.h = (texRect.h / float(texture_->height())) * float(newTexture->width());
		setTexRect(texRect); // it also sets width_ and height_
	}
	else
		setTexRect(Recti(0, 0, newTexture->width(), newTexture->height()));
}

}
