//*********************************************************************************************
// File:			Aeroplane.cpp
// Description:		A very simple class to represent an aeroplane as one object with all the
//					hierarchical components stored internally within the class.
// Module:			Real-Time 3D Techniques for Games
// Created:			Jake - 2010-2011
// Notes:
//*********************************************************************************************

#include "Aeroplane.h"

// Initialise static class variables.
CommonMesh* Aeroplane::s_pPlaneMesh = NULL;
CommonMesh* Aeroplane::s_pPropMesh = NULL;
CommonMesh* Aeroplane::s_pTurretMesh = NULL;
CommonMesh* Aeroplane::s_pGunMesh = NULL;


Aeroplane::Aeroplane(float fX, float fY, float fZ, float fRotY)
{

	std::fill(m_bBulletPool, m_bBulletPool + 100, nullptr);

	m_mCamWorldMatrix = XMMatrixIdentity();

	//creates the plane using the generic hierarchy object
	HierarchicalComponent* m_mGun = new HierarchicalComponent(s_pGunMesh, XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), false);
	HierarchicalComponent* m_mTurret = new HierarchicalComponent(s_pTurretMesh, XMFLOAT4(0.0f, 1.05f, -1.3f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), false);
	//assigns the parents
	m_mTurret->AddChild(m_mGun);
	HierarchicalComponent* m_mProp = new HierarchicalComponent(s_pPropMesh, XMFLOAT4(0.0f, 0.0f, 1.9f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), false);
	HierarchicalComponent* m_mPlane = new HierarchicalComponent(s_pPlaneMesh, XMFLOAT4(fX, fY, fZ, 0.0f), XMFLOAT4(0.0f, fRotY, 0.0f, 0.0f), true);
	m_mPlane->AddChild(m_mProp);
	m_mPlane->AddChild(m_mTurret);

	//sets the root component
	m_RootComponent = m_mPlane;



	m_v4CamOff = XMFLOAT4(0.0f, 4.5f, -15.0f, 0.0f);
	m_v4CamRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_vCamWorldPos = XMVectorZero();
	m_vForwardVector = XMVectorZero();

	m_fSpeed = 0.0f;

	m_bGunCam = false;

	//loads the bullet mesh
	Bullet::LoadResources();

}

Aeroplane::~Aeroplane(void)
{
}
void Aeroplane::LoadResources(void)
{
	s_pPlaneMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/plane.x");
	s_pPropMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/prop.x");
	s_pTurretMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/turret.x");
	s_pGunMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/gun.x");
}

void Aeroplane::ReleaseResources(void)
{
	delete s_pPlaneMesh;
	delete s_pPropMesh;
	delete s_pTurretMesh;
	delete s_pGunMesh;
}


void Aeroplane::UpdateMatrices(void)
{
	XMMATRIX mRotX, mRotY, mRotZ, mTrans;
	XMMATRIX mPlaneCameraRot, mForwardMatrix;


	mRotX = XMMatrixRotationX(XMConvertToRadians(m_v4CamRot.x));
	mRotY = XMMatrixRotationY(XMConvertToRadians(m_v4CamRot.y));
	mRotZ = XMMatrixRotationZ(XMConvertToRadians(m_v4CamRot.z));
	mTrans = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4CamOff));

	//calculate mPlaneCameraRot which ignores rotations in Z and X for the camera to parent to
	mPlaneCameraRot = XMMatrixRotationY(XMConvertToRadians(m_RootComponent->GetRotation().y)) * XMMatrixTranslationFromVector(XMLoadFloat4(&m_RootComponent->GetPosition()));

	m_mCamWorldMatrix = (mRotZ * mRotX * mRotY  * mTrans) * mPlaneCameraRot;


	XMVECTOR zForward = { 0,0,1 };
	//calculates the plane foward vector
	m_vForwardVector = XMVector4Transform(zForward, m_RootComponent->GetWM());

	if (m_bGunCam)
	{
		//swaps the camera to the gun cam
		m_mCamWorldMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4CamOff)) * m_RootComponent->GetChild(1)->GetChild(0)->GetWM();
	}



	// Get the camera's world position (m_vCamWorldPos) out of m_mCameraWorldMatrix
	XMVECTOR empty;
	XMMatrixDecompose(&empty, &empty, &m_vCamWorldPos, m_mCamWorldMatrix);
}

void Aeroplane::Update(bool bPlayerControl)
{
	// DON'T DO THIS UNTIL YOu HAVE COMPLETED THE FUNCTION ABOVE
	if(bPlayerControl)
	{
		//if the speed is above the plane can be controlled
		if (m_fSpeed > 0.4f)
		{
			if (Application::s_pApp->IsKeyPressed('Q'))
			{
				//applies the rotation if the rotation is within the value
				if (m_RootComponent->GetRotation().x > -60)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(-1, 0, 0, 0));
				}
			}
			else
			{
				if (m_RootComponent->GetRotation().x < 0)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(1, 0, 0, 0));
				}
			}
			if (Application::s_pApp->IsKeyPressed('A'))
			{
				if (m_RootComponent->GetRotation().x < 60)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(1, 0, 0, 0));
				}
			}
			else
			{
				if (m_RootComponent->GetRotation().x > 0)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(-1, 0, 0, 0));
				}
			}


			// Step 3: Make the plane yaw and roll left when you press "O" and return to level when released
			// Maximum roll = 20 degrees
			if (Application::s_pApp->IsKeyPressed('O'))
			{
				if (m_RootComponent->GetRotation().z < 20)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(0, 0, 1, 0));
				}
				m_RootComponent->ApplyRotation(XMFLOAT4(0, -1, 0, 0));


			}
			else
			{
				if (m_RootComponent->GetRotation().z > 0)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(0, 0, -1, 0));
				}
			}

			if (Application::s_pApp->IsKeyPressed('P'))
			{
				if (m_RootComponent->GetRotation().z > -20)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(0, 0, -1, 0));

				}
				m_RootComponent->ApplyRotation(XMFLOAT4(0, 1, 0, 0));


			}
			else
			{
				if (m_RootComponent->GetRotation().z < 0)
				{
					m_RootComponent->ApplyRotation(XMFLOAT4(0, 0, 1, 0));
				}
			}
			//fires a bullet
			if (Application::s_pApp->IsKeyPressed(' '))
			{
				//controls the firerate
				if (m_bulletFireRateCounter >= m_bulletFireRate)
				{
					FireBullet(false);
					m_bulletFireRateCounter = 0;
				}
			}
			//fires a rocket
			if (Application::s_pApp->IsKeyPressed('S'))
			{
				if (m_bulletFireRateCounter >= m_bulletFireRate)
				{
					FireBullet(true);
					m_bulletFireRateCounter = 0;
				}
			}
		}
		if (m_bulletFireRateCounter < m_bulletFireRate)
		{
			m_bulletFireRateCounter += GameTimer::deltaTime;
		}
		

		// Apply a forward thrust and limit to a maximum speed of 1
		m_fSpeed += 0.001f;

		if (m_fSpeed > 1)
			m_fSpeed = 1;





	} // End of if player control*/

	//applies the speed to the plane
	m_RootComponent->GetChild(0)->ApplyRotation(XMFLOAT4(0, 0, 100 * m_fSpeed, 0));


	// Tilt gun up and down as turret rotates
	if (m_bGunCam)
	{


		m_RootComponent->GetChild(1)->GetChild(0)->SetRotation(
			XMFLOAT4((sin((float)XMConvertToRadians(m_RootComponent->GetChild(1)->GetRotation().y * 4.0f)) * 10.0f) - 10.0f, 0, 0, 0)
		);

		m_RootComponent->GetChild(1)->ApplyRotation(XMFLOAT4(0, 0.1f, 0, 0 ));

		// Rotate propeller and turret
	}





	  // Move Forward
	XMVECTOR vCurrPos = XMLoadFloat4(&m_RootComponent->GetPosition());
	vCurrPos += m_vForwardVector * m_fSpeed;
	XMFLOAT4 fNewPos;
	XMStoreFloat4(&fNewPos, vCurrPos);


	//applies the forward movement
	m_RootComponent->SetPosition(fNewPos);

	//recursively updates all parts of the plane
	m_RootComponent->Update(nullptr);


	UpdateMatrices();



	//checks if any bullets can be destroyed if not then update it
	for (int i = 0; i < sizeof(m_bBulletPool) / sizeof(*m_bBulletPool); i++)
	{
		if (m_bBulletPool[i] != nullptr)
		{
			if (m_bBulletPool[i]->m_bDestroyFlag)
			{
				delete(m_bBulletPool[i]);
				m_bBulletPool[i] = nullptr;
			}
			else
			{
				m_bBulletPool[i]->Update();
			}
		}
	}
}


void Aeroplane::Draw(void)
{
	m_RootComponent->Draw();

	//draws all available bullets
	for (int i = 0; i < sizeof(m_bBulletPool) / sizeof(*m_bBulletPool); i++)
	{
		if (m_bBulletPool[i] != nullptr)
		{
			m_bBulletPool[i]->Draw();
		}
	}

}

void Aeroplane::FireBullet(bool rocket)
{
	for (int i = 0; i < sizeof(m_bBulletPool) / sizeof(*m_bBulletPool); i++)
	{
		if (m_bBulletPool[i] == nullptr)
		{
			XMVECTOR scale, rot, trans;
			//XMMatrixDecompose(&scale, &rot, &trans, m_RootComponent->GetChild(1)->GetChild(0)->GetWM());


			XMMatrixDecompose(&scale, &rot, &trans, m_RootComponent->GetChild(1)->GetChild(0)->GetWM());
			XMFLOAT4 f3Trans, f3Rot;
			XMStoreFloat4(&f3Trans, trans);

			

			XMVECTOR zForward = { 0,0,1 };

			XMVECTOR m_vGunForwardVector = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m_RootComponent->GetChild(1)->GetChild(0)->GetWM());
			//spawns the bullet in the bullet pool location
			m_bBulletPool[i] = new Bullet(m_vForwardVector, m_vGunForwardVector, m_fSpeed, f3Trans, rot, rocket);
			break;
		}
	}
}