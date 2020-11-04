#ifndef CLASS_NCINE_MESHSPRITE
#define CLASS_NCINE_MESHSPRITE

#include "BaseSprite.h"
#include <nctl/Array.h>

namespace ncine {

/// A scene node representing a mesh with vertices and UVs
class DLL_PUBLIC MeshSprite : public BaseSprite
{
  public:
	/// Vertex data for the mesh
	struct Vertex
	{
		float x, y;
		float u, v;
	};

	enum class TextureCutMode
	{
		RESIZE,
		CROP
	};

	/// Constructor for a sprite with a parent and texture, positioned in the relative origin
	MeshSprite(SceneNode *parent, Texture *texture);
	/// Constructor for a sprite with a texture but no parent, positioned in the origin
	explicit MeshSprite(Texture *texture);
	/// Constructor for a sprite with a parent, a texture and a specified relative position
	MeshSprite(SceneNode *parent, Texture *texture, float xx, float yy);
	/// Constructor for a sprite with a parent, a texture and a specified relative position as a vector
	MeshSprite(SceneNode *parent, Texture *texture, const Vector2f &position);
	/// Constructor for a sprite with a texture and a specified position but no parent
	MeshSprite(Texture *texture, float xx, float yy);
	/// Constructor for a sprite with a texture and a specified position as a vector but no parent
	MeshSprite(Texture *texture, const Vector2f &position);

	/// Returns the number of vertices of the sprite mesh
	inline unsigned int numVertices() const { return numVertices_; }
	/// Returns the vertices data of the sprite mesh
	inline const Vertex *vertices() const { return vertexDataPointer_; }
	/// Returns true if the vertices belong to the sprite and are not stored externally
	inline bool uniqueVertices() const { return vertexDataPointer_ == interleavedVertices_.data(); }
	/// Copies the vertices data from a pointer into the sprite
	void copyVertices(unsigned int numVertices, const Vertex *vertices);
	/// Copies the vertices data from another sprite and sets the same size
	void copyVertices(const MeshSprite &meshSprite);
	/// Sets the vertices data to point to an external array
	void setVertices(unsigned int numVertices, const Vertex *vertices);
	/// Sets the vertices data to the data used by another sprite and sets the same size
	void setVertices(const MeshSprite &meshSprite);
	/// Creates an internal set of vertices from an external array of points in texture space, with optional texture cut mode
	void createVerticesFromTexels(unsigned int numVertices, const Vector2f *points, TextureCutMode cutMode);
	/// Creates an internal set of vertices from an external array of points in texture space
	void createVerticesFromTexels(unsigned int numVertices, const Vector2f *points);

	/// Returns the number of indices used to draw the sprite mesh
	inline unsigned int numIndices() const { return numIndices_; }
	/// Returns the indices used to draw the sprite mesh
	inline const unsigned short *indices() const { return indexDataPointer_; }
	/// Returns true if the indices belong to the sprite and are not stored externally
	inline bool uniqueIndices() const { return indexDataPointer_ == indices_.data(); }
	/// Copies the indices from a pointer into the sprite
	void copyIndices(unsigned int numIndices, const unsigned short *indices);
	/// Copies the indices from another sprite
	void copyIndices(const MeshSprite &meshSprite);
	/// Sets the indices data to point to an external array
	void setIndices(unsigned int numIndices, const unsigned short *indices);
	/// Sets the indices data to the data used by another sprite
	void setIndices(const MeshSprite &meshSprite);

	inline static ObjectType sType() { return ObjectType::MESH_SPRITE; }

  private:
	/// The array of vertex positions interleaved with texture coordinates
	nctl::Array<Vertex> interleavedVertices_;
	/// Pointer to vertex data, either from a shared array or unique to this sprite
	const Vertex *vertexDataPointer_;
	/// The number of vertices, either shared or not, that composes the mesh
	unsigned int numVertices_;

	/// The array of indices used to draw the sprite mesh
	nctl::Array<unsigned short> indices_;
	/// Pointer to index data, either from a shared array or unique to this sprite
	const unsigned short *indexDataPointer_;
	/// The number of indices, either shared or not, that composes the mesh
	unsigned int numIndices_;

	void textureHasChanged(Texture *newTexture) override;
};

}

#endif
