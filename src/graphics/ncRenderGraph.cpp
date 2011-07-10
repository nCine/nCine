#include "ncRenderGraph.h"


///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncRenderGraph::Traverse(ncSceneNode& rNode)
{
	if(!rNode.bShouldDraw)
		return;

	ProcessNode(rNode);

	if(!rNode.m_children.isEmpty())
	{
		const ncListNode<ncSceneNode *> *pListNode = rNode.m_children.Head();

		while (pListNode)
		{
			// List nodes contain pointers, they need deferencing for Traverse()
			ncSceneNode& rChildNode = *(pListNode->Data());
			Traverse(rChildNode);
			pListNode = pListNode->Next();
		}
	}
}

void ncRenderGraph::Draw()
{
	m_sb.Begin();
	if(!m_drawCommands.isEmpty())
	{
		const ncListNode<ncDrawCommand> *pListNode = m_drawCommands.Head();

		while (pListNode)
		{
			const ncDrawCommand& dc = pListNode->Data();
			m_sb.Draw(dc.m_pTexture, dc.m_dstRect, dc.m_srcRect);

			pListNode = pListNode->Next();
		}
	}
	m_sb.End();

	m_drawCommands.Clear();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Process a node for susequent drawing
void ncRenderGraph::ProcessNode(ncSceneNode& rNode)
{
	// Calculating absolute positions
	if (rNode.ParentNode())
	{
		const ncSceneNode& rParent = *(rNode.ParentNode());

		rNode.m_absX = rParent.m_absX + rNode.x;
		rNode.m_absY = rParent.m_absY + rNode.y;
	}
	else
	{
		rNode.m_absX = rNode.x;
		rNode.m_absY = rNode.y;
	}


	if (rNode.Type() == ncSprite::sType())
	{
		ncSprite& sprite = static_cast<ncSprite &>(rNode);
		ncTexture *pTex = sprite.m_pTexture;

		ncRect dstRect = ncRect::FromCenterAndSize(sprite.AbsPosition(), sprite.Size());
		ncRect srcRect(0, 0, 0, 0); // to call the optimized SetTexXoords()

		m_drawCommands.InsertBack(ncDrawCommand(pTex, dstRect, srcRect));
	}
	else
	{

	}
}
