#include <cstdlib> // for exit()
#include "ncSceneNode.h"
#include "ncServiceLocator.h"


///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSceneNode::~ncSceneNode()
{
	if(!m_children.isEmpty())
	{
		ncListNode<ncSceneNode *> *pCurrentNode = m_children.Head();
		ncListNode<ncSceneNode *> *pNextNode = m_children.Head();

		while (pCurrentNode)
		{
			pNextNode = pCurrentNode->Next();
			delete pCurrentNode->Data();
			pCurrentNode = pNextNode;
		}
	}

	if (m_pParent)
	{
		m_pParent->RemoveChildNode(this);
		m_pParent = NULL;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a node as a children of this one
/*!
	\return True if the node has been added
*/
bool ncSceneNode::AddChildNode(ncSceneNode *pChildNode)
{
	bool bAdded = false;

	if(pChildNode)
	{
		if(pChildNode->m_pParent)
			pChildNode->m_pParent->RemoveChildNode(pChildNode);

		pChildNode->m_pParent = this;
		m_children.InsertBack(pChildNode);
		bAdded = true;
	}

	return bAdded;
}

/// Removes a children of this node, without reparenting nephews
/*!
	\return True if the node has been removed
*/
bool ncSceneNode::RemoveChildNode(ncSceneNode *pChildNode)
{
	bool bRemoved = false;

	if(pChildNode && // cannot pass a NULL pointer
		!m_children.isEmpty() && // avoid checking if this node has no children
		pChildNode->m_pParent == this) // avoid checking the child doesn't belong to this one
	{
		pChildNode->m_pParent = NULL;
		m_children.Remove(pChildNode);
		bRemoved = true;
	}

	return bRemoved;
}

/// Removes a children of this node reparenting nephews as children
/*!
	\return True if the node has been unlinked
*/
bool ncSceneNode::UnlinkChildNode(ncSceneNode *pChildNode)
{
	bool bUnlinked = false;

	if(pChildNode && // cannot pass a NULL pointer
		!m_children.isEmpty() && // avoid checking if this node has no children
		pChildNode->m_pParent == this) // avoid checking the child doesn't belong to this one
	{
		pChildNode->m_pParent = NULL;
		m_children.Remove(pChildNode);

		// Nephews reparenting
		ncListNode<ncSceneNode *> *pListNode = pChildNode->m_children.Head();

		while (pListNode)
		{
			ncSceneNode *pNephew = pListNode->Data();
			AddChildNode(pNephew);

			pListNode = pListNode->Next();
		}

		bUnlinked = true;
	}

	return bUnlinked;
}

/// Called once every frame to update the node
void ncSceneNode::Update(unsigned long int ulInterval)
{
	if(!m_children.isEmpty())
	{
		ncListNode<ncSceneNode *> *pListNode = m_children.Head();

		while (pListNode)
		{
			if ((pListNode->Data())->bShouldUpdate)
				(pListNode->Data())->Update(ulInterval);

			pListNode = pListNode->Next();
		}
	}
}

/// Draws the node and visits its children
void ncSceneNode::Visit(ncRenderQueue& rRenderQueue)
{
	// early return if a node is invisible
	if(!bShouldDraw)
		return;

	Transform();
	Draw(rRenderQueue);

	if (!m_children.isEmpty())
	{
		const ncListNode<ncSceneNode *> *pListNode = m_children.Head();

		while (pListNode)
		{
			// List nodes contain pointers, they need deferencing for visiting
			ncSceneNode& rChildNode = *(pListNode->Data());
			rChildNode.Visit(rRenderQueue);
			pListNode = pListNode->Next();
		}
	}
}

/// Returns a pointer to the node with the specified id, if any exists
ncSceneNode* ncSceneNode::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncSceneNode *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncSceneNode::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncSceneNode::FromId - Object not found");
		return NULL;
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void ncSceneNode::Transform()
{
	// Calculating absolute positions
	if (m_pParent)
	{
		m_absX = m_pParent->m_absX + x;
		m_absY = m_pParent->m_absY + y;
	}
	else
	{
		m_absX = x;
		m_absY = y;
	}
}
