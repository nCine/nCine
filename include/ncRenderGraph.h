#ifndef CLASS_NCRENDERGRAPH
#define CLASS_NCRENDERGRAPH

#include "ncList.h"
#include "ncRect.h"
#include "ncTexture.h"
#include "ncSpriteBatch.h"
#include "ncSceneNode.h"
#include "ncSprite.h"

/// Traverse and flatten the scenegraph representation
/*! This class is responsible for traversing the scenegraph
 * and issuing draw commands to the sprite batch
 */
class ncRenderGraph
{
	class ncDrawCommand
	{
	private:
		ncTexture *m_pTexture;
		ncRect m_dstRect;
		ncRect m_srcRect;

		ncDrawCommand(ncTexture *pTexture, ncRect dstRect, ncRect srcRect)
			: m_pTexture(pTexture), m_dstRect(dstRect), m_srcRect(srcRect) { }

		friend class ncRenderGraph;
	};

private:
	ncSpriteBatch m_sb;
	ncList<ncDrawCommand> m_drawCommands;

	void ProcessNode(ncSceneNode& rNode);
public:
	ncRenderGraph() { }
	~ncRenderGraph() { }

	void Traverse(ncSceneNode& rNode);
	void Draw();
};

#endif
