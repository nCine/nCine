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

void ncSceneNode::AddChildNode(ncSceneNode *pChildNode)
{
	if(pChildNode)
	{
		if(pChildNode->m_pParent)
			pChildNode->m_pParent->RemoveChildNode(pChildNode);

		pChildNode->m_pParent = this;
		m_children.InsertBack(pChildNode);
	}
}

void ncSceneNode::RemoveChildNode(ncSceneNode *pChildNode)
{
	if(pChildNode && !m_children.isEmpty())
	{
		pChildNode->m_pParent = NULL;
		m_children.Remove(pChildNode);
	}
}
