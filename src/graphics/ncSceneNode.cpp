#include <cstdlib> // for exit()
#include "ncSceneNode.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const float ncSceneNode::sMinRotation = 0.5f;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSceneNode::ncSceneNode(ncSceneNode* pParent, float fX, float fY)
	: x(fX), y(fY), bShouldUpdate(true), bShouldDraw(true), m_pParent(NULL),
	  m_fScaleFactor(1.0f), m_fRotation(0.0f), m_fAbsX(0.0f), m_fAbsY(0.0f), m_fAbsScaleFactor(1.0f), m_fAbsRotation(0.0f)
{
	m_eType = SCENENODE_TYPE;

	if (pParent)
	{
		pParent->AddChildNode(this);
	}
}

ncSceneNode::ncSceneNode(ncSceneNode* pParent)
	: x(0.0f), y(0.0f), bShouldUpdate(true), bShouldDraw(true), m_pParent(NULL),
	  m_fScaleFactor(1.0f), m_fRotation(0.0f), m_fAbsX(0.0f), m_fAbsY(0.0f), m_fAbsScaleFactor(1.0f), m_fAbsRotation(0.0f)
{
	m_eType = SCENENODE_TYPE;

	if (pParent)
	{
		pParent->AddChildNode(this);
	}
}

ncSceneNode::ncSceneNode()
	: ncObject(), x(0.0f), y(0.0f), bShouldUpdate(true), bShouldDraw(true), m_pParent(NULL),
	  m_fScaleFactor(1.0f), m_fRotation(0.0f), m_fAbsX(0.0f), m_fAbsY(0.0f), m_fAbsScaleFactor(1.0f), m_fAbsRotation(0.0f)
{
	m_eType = SCENENODE_TYPE;
}

ncSceneNode::~ncSceneNode()
{
	ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin();
	while (i != m_children.End())
	{
		delete(*i++);
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

/// Adds a node as a child of this one
/*!
	\return True if the node has been added
*/
bool ncSceneNode::AddChildNode(ncSceneNode *pChildNode)
{
	bool bAdded = false;

	if (pChildNode)
	{
		if (pChildNode->m_pParent)
		{
			pChildNode->m_pParent->RemoveChildNode(pChildNode);
		}

		pChildNode->m_pParent = this;
		m_children.InsertBack(pChildNode);
		bAdded = true;
	}

	return bAdded;
}

/// Removes a child of this node, without reparenting nephews
/*!
	\return True if the node has been removed
*/
bool ncSceneNode::RemoveChildNode(ncSceneNode *pChildNode)
{
	bool bRemoved = false;

	if (pChildNode && // cannot pass a NULL pointer
		!m_children.isEmpty() && // avoid checking if this node has no children
		pChildNode->m_pParent == this) // avoid checking the child doesn't belong to this one
	{
		pChildNode->m_pParent = NULL;
		m_children.Remove(pChildNode);
		bRemoved = true;
	}

	return bRemoved;
}

/// Removes a child of this node while iterating on children, without reparenting nephews
/*!
	It is faster to remove through an iterator than with a linear search for a specific node
	\return True if the node has been removed
*/
bool ncSceneNode::RemoveChildNode(ncList<ncSceneNode *>::Iterator it)
{
	bool bRemoved = false;

	if (*it && // cannot pass a NULL pointer
		!m_children.isEmpty() && // avoid checking if this node has no children
		(*it)->m_pParent == this) // avoid checking the child doesn't belong to this one
	{
		(*it)->m_pParent = NULL;
		m_children.Remove(it);
		bRemoved = true;
	}

	return bRemoved;
}

/// Removes a child of this node reparenting nephews as children
/*!
	\return True if the node has been unlinked
*/
bool ncSceneNode::UnlinkChildNode(ncSceneNode *pChildNode)
{
	bool bUnlinked = false;

	if (pChildNode && // cannot pass a NULL pointer
		!m_children.isEmpty() && // avoid checking if this node has no children
		pChildNode->m_pParent == this) // avoid checking the child doesn't belong to this one
	{
		pChildNode->m_pParent = NULL;
		m_children.Remove(pChildNode);

		// Nephews reparenting
		ncList<ncSceneNode *>::Const_Iterator i = pChildNode->m_children.Begin();
		while (i != pChildNode->m_children.End())
		{
			AddChildNode(*i);
			++i;
		}

		bUnlinked = true;
	}

	return bUnlinked;
}

/// Called once every frame to update the node
void ncSceneNode::Update(float fInterval)
{
	for (ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin(); i != m_children.End(); ++i)
	{
		if ((*i)->bShouldUpdate)
		{
			(*i)->Update(fInterval);
		}
	}
}

/// Draws the node and visits its children
void ncSceneNode::Visit(ncRenderQueue &rRenderQueue)
{
	// early return if a node is invisible
	if (!bShouldDraw)
	{
		return;
	}

	Transform();
	Draw(rRenderQueue);

	for (ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin(); i != m_children.End(); ++i)
	{
		(*i)->Visit(rRenderQueue);
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void ncSceneNode::Transform()
{
	// Calculating absolute transformations
	if (m_pParent)
	{
		m_fAbsScaleFactor = m_pParent->m_fAbsScaleFactor * m_fScaleFactor;
		m_fAbsRotation = m_pParent->m_fAbsRotation + m_fRotation;
		// New scaled position accounting parent scale factor (allow zooming)
		float fScaledX = m_pParent->m_fAbsScaleFactor * x;
		float fScaledY = m_pParent->m_fAbsScaleFactor * y;

		float fSine = 0.0f;
		float fCosine = 1.0f;
		float fParentRot = m_pParent->m_fAbsRotation;
		if (abs(fParentRot) > sMinRotation && abs(fParentRot) < 360.0f - sMinRotation)
		{
			fSine = sinf(-fParentRot * M_PI / 180.0f);
			fCosine = cosf(-fParentRot * M_PI / 180.0f);
		}

		m_fAbsX = m_pParent->m_fAbsX + fScaledX * fCosine - fScaledY * fSine;
		m_fAbsY = m_pParent->m_fAbsY + fScaledY * fCosine + fScaledX * fSine;

		m_absColor = m_pParent->m_absColor * m_color;
	}
	else
	{
		m_fAbsX = x;
		m_fAbsY = y;
		m_fAbsScaleFactor = m_fScaleFactor;
		m_fAbsRotation = m_fRotation;
		m_absColor = m_color;
	}
}
