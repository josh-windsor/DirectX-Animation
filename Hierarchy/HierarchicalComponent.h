#ifndef HIERARCHICALCOMPONENT_H
#define HIERARCHICALCOMPONENT_H
#include "Application.h"
#include <vector>


__declspec(align(16)) class HierarchicalComponent
{
public:
	HierarchicalComponent(CommonMesh* iMeshLoc, XMFLOAT4 iPosition, XMFLOAT4 iRotation, bool iAXYZ);
	~HierarchicalComponent();

	void Update(XMMATRIX* m_ParentMatrix); // Player only has control of plane when flag is set
	void Draw(void);

	//getters for the translation
	XMFLOAT4 GetPosition() { return m_v4Pos; };
	XMFLOAT4 GetRotation() { return m_v4Rot; };

	//applies a float to the current translations
	void ApplyPosition(XMFLOAT4 iPosition) {
		m_v4Pos.x = m_v4Pos.x + iPosition.x;
		m_v4Pos.y = m_v4Pos.y + iPosition.y;
		m_v4Pos.z = m_v4Pos.z + iPosition.z;
		m_v4Pos.w = m_v4Pos.w + iPosition.w;

	}
	void ApplyRotation(XMFLOAT4 iRotation) {
		m_v4Rot.x = m_v4Rot.x + iRotation.x;
		m_v4Rot.y = m_v4Rot.y + iRotation.y;
		m_v4Rot.z = m_v4Rot.z + iRotation.z;
		m_v4Rot.w = m_v4Rot.w + iRotation.w;

	}

	//setters for the translations
	void SetPosition(XMFLOAT4 iPosition) {
		m_v4Pos = iPosition;
	}
	void SetRotation(XMFLOAT4 iRotation) {
		m_v4Rot = iRotation;
	}

	//getters for the frame start positions
	XMFLOAT4 GetStartPos() {
		return m_v4PosStart;
	}
	XMFLOAT4 GetStartRot() {
		return m_v4RotStart;
	}

	//set the frame start vars
	void SetFrameStart() {
		m_v4PosStart = m_v4Pos;
		m_v4RotStart = m_v4Rot;
	}

	//getter for the world matrix
	XMMATRIX GetWM() {
		return m_mWorldMatrix;
	}

	//adds a child to the children vector
	void AddChild(HierarchicalComponent* iChildren) {
		m_HCChildren.push_back(iChildren);
	}

	//gets the child at the specified index
	HierarchicalComponent* GetChild(int index) {
		return m_HCChildren[index];
	}

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}



private:
	//detemines if the transform has the alternate inputs
	bool alternativeXYZ = false;

	std::vector<HierarchicalComponent*> m_HCChildren;

	XMMATRIX m_mWorldMatrix; // World transformation matrix

	void UpdateMatrices(XMMATRIX* m_ParentMatrix);
	XMFLOAT4 m_v4Rot; // Euler rotation angles
	XMFLOAT4 m_v4Pos; // World position

	XMFLOAT4 m_v4RotStart;
	XMFLOAT4 m_v4PosStart;


	CommonMesh* m_Mesh = nullptr; // Only one plane mesh for all instances


	XMMATRIX m_mWorldTrans; // World translation matrix

};

#endif