#pragma once
#include "Application.h"

__declspec(align(16)) class Bullet
{
public:
	//bullet constructor
	Bullet(XMVECTOR vPlaneForwardVec, XMVECTOR vGunForwardVec, float fPlaneSpeed, XMFLOAT4 iPosition, XMVECTOR iRotation, bool iRocket);
	~Bullet();

	static void LoadResources(void); // Only load the resources once for all instances
	static void ReleaseResources(void); // Only free the resources once for all instances
	void Update(); 
	void Draw(void);
	//the flag to destroy this bullet
	bool m_bDestroyFlag = false;


private:
	void UpdateMatrices(void);

	//the rocket timer
	float m_timer = 0;

	//the life of the bullet 
	float m_Life = 0;

	static CommonMesh* s_pBulletMesh; 
	static CommonMesh* s_pRocketMesh; 
	XMVECTOR m_v4Rot; // Euler rotation angles
	XMFLOAT4 m_v4Pos; // World position
	XMFLOAT3 m_v3Scale; // World position

	XMMATRIX m_mWorldMatrix; // World transformation matrix

	XMVECTOR m_v4LastRot; // Euler rotation angles

	//forward vectors
	XMVECTOR m_vPlaneForwardVector;
	XMVECTOR m_vGunForwardVector;
	float m_fPlaneSpeed;

	//lerp function for rocket
	void LerpRot(XMVECTOR currentRot, XMVECTOR endRot, float time);

	//if the bullet is a rocket with tracking
	bool rocket;

public:


	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

};

