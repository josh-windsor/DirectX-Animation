#ifndef AEROPLANE_H
#define AEROPLANE_H

//*********************************************************************************************
// File:			Aeroplane.h
// Description:		A very simple class to represent an aeroplane as one object with all the
//					hierarchical components stored internally within the class.
// Module:			Real-Time 3D Techniques for Games
// Created:			Jake - 2010-2011
// Notes:
//*********************************************************************************************

#include "Application.h"
#include "Bullet.h"
#include "HierarchicalComponent.h"
#include <vector>


__declspec(align(16)) class Aeroplane
{
  public:
	Aeroplane(float fX = 0.0f, float fY = 0.0f, float fZ = 0.0f, float fRotY = 0.0f);
	~Aeroplane(void);

	void Update(bool bPlayerControl); // Player only has control of plane when flag is set
	void Draw(void);

	static void LoadResources(void); // Only load the resources once for all instances
	static void ReleaseResources(void); // Only free the resources once for all instances


	HierarchicalComponent* GetRoot() {
		return m_RootComponent;
	}

  private:
	void UpdateMatrices(void);

	static CommonMesh* s_pPlaneMesh; // Only one plane mesh for all instances
	static CommonMesh* s_pPropMesh; // Only one propellor mesh for all instances
	static CommonMesh* s_pTurretMesh; // Only one turret mesh for all instances
	static CommonMesh* s_pGunMesh; // Only one gun mesh for all instances


	//function to fire a bullet from the gun
	void FireBullet(bool rocket);

	//the root of the plane
	HierarchicalComponent* m_RootComponent;

	XMVECTOR m_vForwardVector; // Forward Vector for Plane
	float m_fSpeed; // Forward speed

	XMFLOAT4 m_v4CamRot; // Local rotation angles
	XMFLOAT4 m_v4CamOff; // Local offset

	XMVECTOR m_vCamWorldPos; // World position
	XMMATRIX m_mCamWorldMatrix; // Camera's world transformation matrix


	// if the camera is in gun mode
	bool m_bGunCam;


	//some bullet vars
	// the pool of bullet pointers initilised to nullptr
	Bullet *m_bBulletPool[100];
	//the fire rate reset of the gun
	float m_bulletFireRate = 0.25f;
	//the current time since last bullet
	float m_bulletFireRateCounter = 0;

  public:
	XMFLOAT4 GetFocusPosition(void) { return GetPosition(); }
	XMFLOAT4 GetCameraPosition(void)
	{
		XMFLOAT4 v4Pos;
		XMStoreFloat4(&v4Pos, m_vCamWorldPos);
		return v4Pos;
	}
	XMFLOAT4 GetPosition(void) { return m_RootComponent->GetPosition(); }
	void SetGunCamera(bool value) { m_bGunCam = value; }

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
};

#endif