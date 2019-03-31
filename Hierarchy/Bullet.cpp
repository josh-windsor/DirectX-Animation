#include "Bullet.h"

CommonMesh* Bullet::s_pBulletMesh = NULL;
CommonMesh* Bullet::s_pRocketMesh = NULL;


Bullet::Bullet(XMVECTOR vPlaneForwardVec, XMVECTOR vGunForwardVec, float fPlaneSpeed, XMFLOAT4 iPosition, XMVECTOR iRotation, bool iRocket)
	: m_vPlaneForwardVector(vPlaneForwardVec),
	  m_vGunForwardVector(vGunForwardVec),
	  m_fPlaneSpeed(fPlaneSpeed),
	  m_v4Rot(iRotation),
	  m_v4Pos(iPosition),
	  rocket(iRocket)
{
	//inits some vars
	m_mWorldMatrix = XMMatrixIdentity();
	m_timer = 0;



	//changes scale if its a rocket
	if (iRocket)
	{
		m_v3Scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	}
	else
	{
		m_v3Scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
	}
}


Bullet::~Bullet()
{
}
void Bullet::UpdateMatrices(void)
{

	XMMATRIX mScale, mTrans, mRot;

	//applies the scale transform and rotation to the bullet
	mScale = XMMatrixScaling(m_v3Scale.x, m_v3Scale.y, m_v3Scale.z);
	mTrans = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4Pos));
	mRot = XMMatrixRotationQuaternion(m_v4Rot);


	m_mWorldMatrix = mScale * mRot * mTrans;


}



void Bullet::Update()
{
	//if the bullet is still alive
	if (m_Life < 10)
	{
		//add onto life
		m_Life += GameTimer::deltaTime;
		if (rocket)
		{
			m_timer += GameTimer::deltaTime;

			//launch straight for the first second
			if (m_timer < 1)
			{
				XMVECTOR vCurrPos2 = XMLoadFloat4(&m_v4Pos);

				//goes in the direction of the gun by the plane speed
				vCurrPos2 += (m_vPlaneForwardVector + m_vGunForwardVector) * m_fPlaneSpeed;

				XMStoreFloat4(&m_v4Pos, vCurrPos2);

				//stores the final rotation before lerping
				m_v4LastRot = m_v4Rot;
			}
			else
			{
				//creates a timer for the lerp between 0 and 1
				float timer2 = (m_timer - 1) / 2;

				//creates a lookat vector towards the robot
				XMVECTOR lookat = Application::s_pApp->ReturnRobotPos();
				XMVECTOR vCurrPos = XMLoadFloat3(&XMFLOAT3(m_v4Pos.x, m_v4Pos.y, m_v4Pos.z));
				XMVECTOR objectUpVector = { 0.0f, 1.0f, 0.0f };

				//calculates the direction to the robot
				XMVECTOR VectorZ = XMVector3Normalize(lookat - vCurrPos);
				XMVECTOR VectorX = XMVector3Normalize(XMVector3Cross(objectUpVector, VectorZ));
				XMVECTOR VectorY = XMVector3Cross(VectorZ, VectorX);

				XMFLOAT3 zaxis, xaxis, yaxis;
				XMStoreFloat3(&zaxis, VectorZ);
				XMStoreFloat3(&yaxis, VectorY);
				XMStoreFloat3(&xaxis, VectorX);

				//builds a orientation matrix toward the enemy
				XMMATRIX orientationMat(
					xaxis.x, xaxis.y, xaxis.z, 0,
					yaxis.x, yaxis.y, yaxis.z, 0,
					zaxis.x, zaxis.y, zaxis.z, 0,
					0, 0, 0, 1
				);

				XMVECTOR empty1, endingRot,empty2;
				//gets the rotation out of the above matrix
				XMMatrixDecompose(&empty1, &endingRot, &empty2, orientationMat);

				//lerps between the last straight rotation and the robot direction rotation
				LerpRot(m_v4LastRot, endingRot, timer2);

				//moves the bullet forward consistantly on its own foward vector
				XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m_mWorldMatrix);
				vCurrPos += forward * m_fPlaneSpeed * 5;

				XMStoreFloat4(&m_v4Pos, vCurrPos);

			}
			

		}
		else
		{
			//normal bullet code
			XMVECTOR vCurrPos = XMLoadFloat4(&m_v4Pos);
			//moves in the direction the gun fired
			vCurrPos += (m_vPlaneForwardVector + m_vGunForwardVector) * m_fPlaneSpeed;

			XMStoreFloat4(&m_v4Pos, vCurrPos);


		}
		//calculates if the plane is within attacking distance of the robot
		XMVECTOR robotVec = Application::s_pApp->ReturnRobotPos();

		XMVECTOR posVec = XMLoadFloat4(&m_v4Pos);
		 


		XMVECTOR difference = robotVec - posVec;

		float distance;
		XMStoreFloat(&distance, XMVector4Dot(difference, difference));

		//starts attacking if within distance
		if (distance < 25)
		{
			m_bDestroyFlag = true;
			Application::s_pApp->KillRobot();
		}


		UpdateMatrices();
		


	}
	else
	{
		m_bDestroyFlag = true;
	}

}

void Bullet::LerpRot(XMVECTOR currentRot, XMVECTOR endRot, float time)
{
	//if within time then lerp
	if (time < 1)
	{
		m_v4Rot = XMVector4Normalize(XMVectorLerp(currentRot, endRot, time));
	}
	else
	{
		//set to end if not
		m_v4Rot = endRot;
	}

}

void Bullet::LoadResources(void)
{
	s_pBulletMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/bullet.x");
	s_pRocketMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/rocket.x");
}

void Bullet::ReleaseResources(void)
{
	delete s_pBulletMesh;
	delete s_pRocketMesh;
}

void Bullet::Draw(void)
{
	if (!m_bDestroyFlag)
	{
		Application::s_pApp->SetWorldMatrix(m_mWorldMatrix);
		if (rocket)
		{
			s_pRocketMesh->Draw();
		}
		else
		{
			s_pBulletMesh->Draw();
		}

	}
}


