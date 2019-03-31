#include "HierarchicalComponent.h"


HierarchicalComponent::HierarchicalComponent(CommonMesh* iMeshLoc, XMFLOAT4 iPosition, XMFLOAT4 iRotation, bool iAXYZ)
{
	m_mWorldMatrix = XMMatrixIdentity();
	//sets the mesh if it has one
	if (iMeshLoc != nullptr)
	{
		m_Mesh = iMeshLoc;
	}

	//sets the input vars
	m_v4Pos = iPosition;
	m_v4Rot = iRotation;
	alternativeXYZ = iAXYZ;
}


HierarchicalComponent::~HierarchicalComponent()
{
}

void HierarchicalComponent::UpdateMatrices(XMMATRIX* m_ParentMatrix)
{
	XMMATRIX mRotX, mRotY, mRotZ, mTrans;

	//applies the current rotation and transform
	mRotX = XMMatrixRotationX(XMConvertToRadians(m_v4Rot.x));
	mRotY = XMMatrixRotationY(XMConvertToRadians(m_v4Rot.y));
	mRotZ = XMMatrixRotationZ(XMConvertToRadians(m_v4Rot.z));
	mTrans = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4Pos));
	//if its using an alterative order to avoid gimble lock on plane
	if (alternativeXYZ)
	{
		if (m_ParentMatrix != nullptr)
		{
			//parents the world matrix if it has a parent
			m_mWorldMatrix = (mRotZ * mRotX * mRotY * mTrans) * *m_ParentMatrix;
		}
		else
		{
			m_mWorldMatrix = (mRotZ * mRotX * mRotY * mTrans);
		}
	}
	else
	{
		if (m_ParentMatrix != nullptr)
		{
			m_mWorldMatrix = (mRotX * mRotY * mRotZ * mTrans) * *m_ParentMatrix;
		}
		else
		{
			m_mWorldMatrix = (mRotX * mRotY * mRotZ * mTrans);
		}
	}
}
void HierarchicalComponent::Update(XMMATRIX* m_ParentMatrix)
{
	//updates the matricies with its above parent if it has one
	UpdateMatrices(m_ParentMatrix);
	//recursively calls all of its children to update
	if (m_HCChildren.size() > 0) {
		for each (HierarchicalComponent* child in m_HCChildren)
		{
			child->Update(&m_mWorldMatrix);
		}
	}

}

void HierarchicalComponent::Draw(void)
{
	//draws the object
	Application::s_pApp->SetWorldMatrix(m_mWorldMatrix);
	//if it has a mesh then draw
	if (m_Mesh != nullptr)
	{
		m_Mesh->Draw();
	}
	//recursively calls all of its children to draw
	if (m_HCChildren.size() > 0)
	{
		for each (HierarchicalComponent* child in m_HCChildren)
		{
			child->Draw();
		}

	}
}