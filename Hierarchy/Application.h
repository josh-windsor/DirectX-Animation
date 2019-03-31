#ifndef APPLICATION_H
#define APPLICATION_H

#define WIN32_LEAN_AND_MEAN

#include <assert.h>

#include <stdio.h>
#include <windows.h>
#include <d3d11.h>

#include "CommonApp.h"
#include "CommonMesh.h"
#include "GameTimer.h"

class Aeroplane;
class Robot;
class HeightMap;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Application : public CommonApp
{
  public:
	static Application* s_pApp;

	XMVECTOR ReturnPlanePos();
	XMVECTOR ReturnRobotPos();
	void KillRobot();


  protected:
	bool HandleStart();
	void HandleStop();
	void HandleUpdate();
	void HandleRender();

  private:
	int  m_frameCount = 0;


	float m_rotationAngle;
	float m_cameraZ;
	bool m_bWireframe;

	int m_cameraState;

	Aeroplane* m_pAeroplane;
	Robot* m_pRobot;
	HeightMap* m_pHeightMap;

	void ReloadShaders();

};

#endif
