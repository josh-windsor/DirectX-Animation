#ifndef ROBOT_H
#define ROBOT_H

#include "Application.h"
#include "HierarchicalComponent.h"
#include "AnimationController.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

//struct containing each robot part details
struct RobotPart
{
	std::string locationName;
	//the robots parent location name
	std::string parentName;
	XMFLOAT4 location;
};

__declspec(align(16)) class Robot
{
public:
	Robot();
	~Robot();

	void Update(); 
	void Draw(void);
	static void ReleaseResources(void); // Only free the resources once for all instances

	void Die();

	XMFLOAT4 m_v4CamRot; // Local rotation angles
	XMFLOAT4 m_v4CamOff; // Local offset


	XMVECTOR m_vCamWorldPos; // World position
	XMMATRIX m_mCamWorldMatrix; // Camera's world transformation matrix

	void TransitionAnimation(int animation);

	//Camera focus functions
	XMFLOAT4 GetFocusPosition(void) { return m_RootComponent->GetPosition(); }
	XMFLOAT4 GetCameraPosition(void)
	{
		XMFLOAT4 v4Pos;
		XMStoreFloat4(&v4Pos, m_vCamWorldPos);
		return v4Pos;
	}
	XMFLOAT4 GetPosition(void) { return m_RootComponent->GetPosition(); }

	//returns the root component
	HierarchicalComponent* GetRoot() {
		return m_RootComponent;
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

	//Resets all the global animation variables
	void ResetGlobals(bool endable);

	static vector<CommonMesh*> meshContainer;

	void UpdateMatrices(void);

	//pointer to the top level component
	HierarchicalComponent* m_RootComponent;

	//a pointer to the attached animation controller
	AnimationController* animController;

	//Plays a single animation frame
	void PlayAnimationFrame(int animation);

	//contains all components vs their location name for easy access
	std::map<std::string, HierarchicalComponent*> components;

	//the interpolation functions
	void LerpPos(HierarchicalComponent* currentHc, KeyFrame* currentKF, int animation, string partName, float time);
	void LerpRot(HierarchicalComponent* currentHc, KeyFrame* currentKF, int animation, string partName, float time);

	//A collection of all the robot parts
	RobotPart m_robotParts[16];

	void Debug(KeyFrame* currentKF, HierarchicalComponent* currentHc, RobotPart part);

	//if the plane is close
	bool withinPlane;
	bool pressed = false;



	//Animation variables
	int animationCurrentFrame = 0;
	int currentAnimation = 0;
	float startOfFrameTimer = 0;
	float endOfFrameTimer = 0;
	float animationTimer = 0;
	bool animationEnded = false;
	bool endableAnim = false;
	bool debugMode = false;
	float animSpeedTimer = 0;
	bool newAnim = false;
	float transitionTimer = 0;
	float transitionTime = 0.2f;

};

#endif