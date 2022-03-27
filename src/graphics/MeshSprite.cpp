#include <cstring> // for memcpy()
#include "MeshSprite.h"
#include "RenderCommand.h"
#include "RenderResources.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

MeshSprite::MeshSprite()
    : MeshSprite(nullptr, nullptr, 0.0f, 0.0f)
{
}

MeshSprite::MeshSprite(SceneNode *parent, Texture *texture)
    : MeshSprite(parent, texture, 0.0f, 0.0f)
{
}

MeshSprite::MeshSprite(Texture *texture)
    : MeshSprite(nullptr, texture, 0.0f, 0.0f)
{
}

MeshSprite::MeshSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : BaseSprite(parent, texture, xx, yy),
      vertices_(16), vertexDataPointer_(nullptr), numVertices_(0),
      indices_(16), indexDataPointer_(nullptr), numIndices_(0)
{
	init();
}

MeshSprite::MeshSprite(SceneNode *parent, Texture *texture, const Vector2f &position)
    : MeshSprite(parent, texture, position.x, position.y)
{
}

MeshSprite::MeshSprite(Texture *texture, float xx, float yy)
    : MeshSprite(nullptr, texture, xx, yy)
{
}

MeshSprite::MeshSprite(Texture *texture, const Vector2f &position)
    : MeshSprite(nullptr, texture, position.x, position.y)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void MeshSprite::copyVertices(unsigned int numVertices, const float *vertices)
{
	const unsigned int numBytes = texture_ ? sizeof(Vertex) : sizeof(VertexNoTexture);
	const unsigned int numFloats = numBytes / sizeof(float);

	vertices_.setSize(numVertices * numFloats);
	memcpy(vertices_.data(), vertices, numVertices * numBytes);

	vertexDataPointer_ = vertices_.data();
	numVertices_ = numVertices;
	renderCommand_->geometry().setNumVertices(numVertices);
	renderCommand_->geometry().setHostVertexPointer(vertexDataPointer_);
}

void MeshSprite::copyVertices(unsigned int numVertices, const Vertex *vertices)
{
	copyVertices(numVertices, reinterpret_cast<const float *>(vertices));
}

void MeshSprite::copyVertices(unsigned int numVertices, const VertexNoTexture *vertices)
{
	copyVertices(numVertices, reinterpret_cast<const float *>(vertices));
}

void MeshSprite::copyVertices(const MeshSprite &meshSprite)
{
	copyVertices(meshSprite.numVertices_, meshSprite.vertexDataPointer_);
	width_ = meshSprite.width_;
	height_ = meshSprite.height_;
	texRect_ = meshSprite.texRect_;

	dirtyBits_.set(DirtyBitPositions::SizeBit);
	dirtyBits_.set(DirtyBitPositions::AabbBit);
	dirtyBits_.set(DirtyBitPositions::TextureBit);
}

void MeshSprite::setVertices(unsigned int numVertices, const float *vertices)
{
	vertices_.clear();

	vertexDataPointer_ = vertices;
	numVertices_ = numVertices;
	renderCommand_->geometry().setNumVertices(numVertices);
	renderCommand_->geometry().setHostVertexPointer(vertexDataPointer_);
}

void MeshSprite::setVertices(unsigned int numVertices, const Vertex *vertices)
{
	copyVertices(numVertices, reinterpret_cast<const float *>(vertices));
}

void MeshSprite::setVertices(unsigned int numVertices, const VertexNoTexture *vertices)
{
	copyVertices(numVertices, reinterpret_cast<const float *>(vertices));
}

void MeshSprite::setVertices(const MeshSprite &meshSprite)
{
	setVertices(meshSprite.numVertices_, meshSprite.vertexDataPointer_);
	width_ = meshSprite.width_;
	height_ = meshSprite.height_;

	dirtyBits_.set(DirtyBitPositions::SizeBit);
	dirtyBits_.set(DirtyBitPositions::AabbBit);
}

void MeshSprite::createVerticesFromTexels(unsigned int numVertices, const Vector2f *points, TextureCutMode cutMode)
{
	FATAL_ASSERT(numVertices >= 3);

	const unsigned int numFloats = texture_ ? VertexFloats : VertexNoTextureFloats;
	vertices_.setSize(numVertices * numFloats);
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
	else if (texRect_.w > 0 && texRect_.h > 0)
	{
		width_ = static_cast<float>(texRect_.w);
		height_ = static_cast<float>(texRect_.h);
	}

	const float halfWidth = width_ * 0.5f;
	const float halfHeight = height_ * 0.5f;

	for (unsigned int i = 0; i < numVertices; i++)
	{
		if (texture_)
		{
			Vertex &v = reinterpret_cast<Vertex &>(vertices_[i * VertexFloats]);
			v.x = (points[i].x - min.x - halfWidth) / width_; // from -0.5 to 0.5
			v.y = (points[i].y - min.y - halfHeight) / height_; // from -0.5 to 0.5
			v.u = points[i].x / (texRect_.w - texRect_.x);
			v.v = (texRect_.h - points[i].y) / (texRect_.h - texRect_.y); // flipped
		}
		else
		{
			VertexNoTexture &v = reinterpret_cast<VertexNoTexture &>(vertices_[i * VertexNoTextureFloats]);
			v.x = (points[i].x - min.x - halfWidth) / width_; // from -0.5 to 0.5
			v.y = (points[i].y - min.y - halfHeight) / height_; // from -0.5 to 0.5
		}
	}

	vertexDataPointer_ = vertices_.data();
	numVertices_ = numVertices;
	renderCommand_->geometry().setNumVertices(numVertices);
	renderCommand_->geometry().setHostVertexPointer(vertexDataPointer_);

	dirtyBits_.set(DirtyBitPositions::SizeBit);
	dirtyBits_.set(DirtyBitPositions::AabbBit);
}

void MeshSprite::createVerticesFromTexels(unsigned int numVertices, const Vector2f *points)
{
	createVerticesFromTexels(numVertices, points, TextureCutMode::RESIZE);
}

void MeshSprite::copyIndices(unsigned int numIndices, const unsigned short *indices)
{
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
	indices_.clear();

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
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

MeshSprite::MeshSprite(const MeshSprite &other)
    : BaseSprite(other)
{
	init();
	setTexRect(other.texRect_);
	copyVertices(other.numVertices_, other.vertices_.data());
	copyIndices(other.numIndices_, other.indices_.data());
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void MeshSprite::init()
{
	ZoneScoped;
	if (texture_ && texture_->name() != nullptr)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(texture_->name(), nctl::strnlen(texture_->name(), Object::MaxNameLength));
	}

	type_ = ObjectType::MESH_SPRITE;
	renderCommand_->setType(RenderCommand::CommandTypes::MESH_SPRITE);

	const Material::ShaderProgramType shaderProgramType = [](Texture *texture)
	{
		if (texture)
			return (texture->numChannels() >= 3) ? Material::ShaderProgramType::MESH_SPRITE
			                                     : Material::ShaderProgramType::MESH_SPRITE_GRAY;
		else
			return Material::ShaderProgramType::MESH_SPRITE_NO_TEXTURE;
	}(texture_);
	renderCommand_->material().setShaderProgramType(shaderProgramType);

	shaderHasChanged();
	renderCommand_->geometry().setPrimitiveType(GL_TRIANGLE_STRIP);
	renderCommand_->geometry().setNumElementsPerVertex(texture_ ? VertexFloats : VertexNoTextureFloats);
	renderCommand_->geometry().setHostVertexPointer(vertexDataPointer_);

	if (texture_)
		setTexRect(Recti(0, 0, texture_->width(), texture_->height()));
}

void MeshSprite::shaderHasChanged()
{
	BaseSprite::shaderHasChanged();
	renderCommand_->material().setDefaultAttributesParameters();
}

void MeshSprite::textureHasChanged(Texture *newTexture)
{
	if (renderCommand_->material().shaderProgramType() != Material::ShaderProgramType::CUSTOM)
	{
		const Material::ShaderProgramType shaderProgramType = [](Texture *texture)
		{
			if (texture)
				return (texture->numChannels() >= 3) ? Material::ShaderProgramType::MESH_SPRITE
				                                     : Material::ShaderProgramType::MESH_SPRITE_GRAY;
			else
				return Material::ShaderProgramType::MESH_SPRITE_NO_TEXTURE;
		}(newTexture);
		const bool hasChanged = renderCommand_->material().setShaderProgramType(shaderProgramType);
		if (hasChanged)
			shaderHasChanged();
	}

	renderCommand_->geometry().setNumElementsPerVertex(newTexture ? VertexFloats : VertexNoTextureFloats);

	if (texture_ && newTexture && texture_ != newTexture)
	{
		Recti texRect = texRect_;
		texRect.x = (texRect.x / float(texture_->width())) * float(newTexture->width());
		texRect.y = (texRect.y / float(texture_->height())) * float(newTexture->width());
		texRect.w = (texRect.w / float(texture_->width())) * float(newTexture->width());
		texRect.h = (texRect.h / float(texture_->height())) * float(newTexture->width());
		setTexRect(texRect); // it also sets width_ and height_
	}
	else if (texture_ == nullptr && newTexture)
	{
		// Assigning a texture when there wasn't any
		setTexRect(Recti(0, 0, newTexture->width(), newTexture->height()));
	}
}

}
