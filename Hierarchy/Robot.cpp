#include "Robot.h"



vector<CommonMesh*> Robot::meshContainer = vector<CommonMesh*>{};


Robot::Robot()
{
	//reads in the hierarchy text file
	std::ifstream infile("Resources/Robot/hierarchy.txt");
	std::string line;
	int i = 0;
	int j = 0;
	//while the file still has lines
	while (std::getline(infile, line))
	{
		//removes the quotation marks
		line.erase(remove(line.begin(), line.end(), '"'), line.end());

		//switches based on the line
		switch (i)
		{
		case 0:
			//gets the part's name
			m_robotParts[j].locationName = line;
			i++;
			break;
		case 1:
			//gets the parent part's name
			m_robotParts[j].parentName = line;
			i++;
			break;
		case 2:
			int k = 0;
			std::string xyz[3] = {"", "", ""};
			//loops through each character
			for (char& c : line) {
				switch (c)
				{
				case '"':
					continue;
					break;
				case ' ':
					continue;
					//delimits at a comma and adds to the next location in the string array
				case ',':
					k++;
					continue;

				default:
					//adds to the current array location
					xyz[k] += c;
					break;
				}
			}
			//converts the string to floats, divides them by 10 and stores as the location 
			m_robotParts[j].location = XMFLOAT4(atof(xyz[0].c_str()) /10 ,atof(xyz[1].c_str())/10, atof(xyz[2].c_str())/10, 0);
			i = 0;
			j++;
			break;
		}
	}

	//loops through all the robot parts from the text file
	for each (RobotPart part in m_robotParts)
	{
		HierarchicalComponent* hC;
		//if its not the root
		if (part.parentName != "")
		{
			//gets the mesh location
			std::string fileLoc = "Resources/Robot/" + part.locationName + ".x";
			CommonMesh* mesh = CommonMesh::LoadFromXFile(Application::s_pApp, fileLoc.c_str());
			meshContainer.push_back(mesh);
			//builds a component
			hC = new HierarchicalComponent(mesh, part.location, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), false);
		}
		else
		{
			//if its the root
			hC = new HierarchicalComponent(nullptr, part.location, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), false);
			//add another level to wrap the robot with a location
			HierarchicalComponent* hCroot = new HierarchicalComponent(nullptr, XMFLOAT4(part.location.x, (part.location.y - 5), (part.location.z - 20), part.location.w), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), false);
			//sets the root to this new level above root
			hCroot->AddChild(hC);
			m_RootComponent = hCroot;
		}
		//inserts into the map using its name vs the component
		components.insert(std::pair<std::string, HierarchicalComponent*>(part.locationName, hC));
	}
	for each (RobotPart part in m_robotParts) {
		if (part.parentName != "")
		{
			//sets all the children using the map
			components[part.parentName]->AddChild(components[part.locationName]);
		}
	}

	m_mCamWorldMatrix = XMMatrixIdentity();

	m_v4CamOff = XMFLOAT4(0.0f, 10.0f, 50.0f, 0.0f);
	m_v4CamRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_vCamWorldPos = XMVectorZero();

	animController = new AnimationController();
	//sets the frame start location for each robot part
	for each (RobotPart part in m_robotParts)
	{
		components[part.locationName]->SetFrameStart();

	}


}


Robot::~Robot()
{
}
void Robot::UpdateMatrices(void) {
	XMMATRIX mRotX, mRotY, mRotZ, mTrans;
	XMMATRIX mPlaneCameraRot, mForwardMatrix;

	//calculates and updates the robot camera
	mRotX = XMMatrixRotationX(XMConvertToRadians(m_v4CamRot.x));
	mRotY = XMMatrixRotationY(XMConvertToRadians(m_v4CamRot.y));
	mRotZ = XMMatrixRotationZ(XMConvertToRadians(m_v4CamRot.z));
	mTrans = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4CamOff));

	m_mCamWorldMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4CamOff)) * m_RootComponent->GetWM();


	XMVECTOR vector1, vector2;
	XMMatrixDecompose(&vector1, &vector2, &m_vCamWorldPos, m_mCamWorldMatrix);


}

void Robot::Update() {
	
	//calculates if the plane is within attacking distance of the robot
	XMVECTOR planeVec = Application::s_pApp->ReturnPlanePos();
	XMVECTOR robotVec = XMLoadFloat4(&m_RootComponent->GetPosition());

	XMVECTOR difference = robotVec - planeVec;
	XMVECTOR out = XMVector4Dot(difference, difference);

	float distance;
	XMStoreFloat(&distance, out);

	//starts attacking if within distance
	if (currentAnimation != 1)
	{
		if (distance < 1000 && !withinPlane)
		{
			withinPlane = true;
			currentAnimation = 2;
			ResetGlobals(false);
		}
		//stops attacking when further away
		else if (distance > 1000 && withinPlane)
		{
			withinPlane = false;
			currentAnimation = 0;
			ResetGlobals(false);

		}

	}



	if (debugMode)
	{
		animSpeedTimer += GameTimer::deltaTime;
		//splits into 5 frames a second for debugging (change this to 0.5 for 2 fps)
		if (animSpeedTimer >= 0.2f)
		{
			animSpeedTimer = 0;
			PlayAnimationFrame(currentAnimation);
		}
	}
	else
	{
		//animates at full speed
		animSpeedTimer = 0;
		
		//decides if the transition should occur
		if (!newAnim)
		{
			PlayAnimationFrame(currentAnimation);
		}
		else
		{
			for each (RobotPart part in m_robotParts)
			{
				//sets the frame start to lerp from
				components[part.locationName]->SetFrameStart();

			}

			//ends the transition
			if (transitionTimer > transitionTime)
			{
				newAnim = false;
				transitionTimer = 0;

			}
			else
			{
				TransitionAnimation(currentAnimation);

			}

			transitionTimer += GameTimer::deltaTime;


		}
	}

	//starts debug mode on keypress of f
	if (Application::s_pApp->IsKeyPressed('F'))
	{
		if (!pressed)
		{
			pressed = true;
			debugMode = !debugMode;
		}
	}
	else
	{
		pressed = false;
	}

	//swaps between the different animations
	if (Application::s_pApp->IsKeyPressed('1'))
	{
		currentAnimation = 0;
		ResetGlobals(false);

	}
	else if (Application::s_pApp->IsKeyPressed('2'))
	{
		currentAnimation = 1;
		ResetGlobals(true);
	}
	else if (Application::s_pApp->IsKeyPressed('3'))
	{
		currentAnimation = 2;
		ResetGlobals(false);

	}
	
	//recursively updates all robot parts
	m_RootComponent->Update(nullptr);

	UpdateMatrices();
}

void Robot::ReleaseResources(void)
{
	for each (CommonMesh* mesh in meshContainer)
	{
		delete(mesh);
	}
}

void Robot::Draw() 
{
	m_RootComponent->Draw();
}
//resets all the global animation vars
void Robot::ResetGlobals(bool endable) {
	animationCurrentFrame = 0;
	animationTimer = 0;
	animationEnded = false;
	endableAnim = endable;
	newAnim = true;
}


void Robot::PlayAnimationFrame(int animation)
{
	//adds to the total animation timer and the frame timer
	animationTimer += GameTimer::deltaTime;
	startOfFrameTimer += GameTimer::deltaTime;
	//loops through all the robot parts
	for each (RobotPart part in m_robotParts) {
		//gets the number of frames to execute
		int numberOfFrames = animController->animTimings[animation][part.locationName].size();
		//checks if the animation should play the next frame
		if (animationTimer >= animController->animTimings[animation][part.locationName][animationCurrentFrame])
		{
			//resets the frame timer
			startOfFrameTimer = 0;
			//adds to the current frame
			animationCurrentFrame++;

			//sets the frame start for each part
			for each (RobotPart part in m_robotParts) 
			{
				components[part.locationName]->SetFrameStart();
			}
			//check if its the the last frame or past
			if (animationCurrentFrame >= numberOfFrames)
			{
				//sets the end time to be the first frame time
				endOfFrameTimer = animController->animTimings[animation][part.locationName][0];
			}
			else
			{
				//sets the end time to the current frame - the previous frame
				endOfFrameTimer = animController->animTimings[animation][part.locationName][animationCurrentFrame] - animController->animTimings[animation][part.locationName][animationCurrentFrame - 1];
			}
		}
		//check if its the the last frame or past
		if (animationCurrentFrame >= numberOfFrames)
		{
			//resets to the beginning of the animation
			animationCurrentFrame = 0;
			animationTimer = 0;
			
			//ends the animation if it shouldn't loop
			if (endableAnim)
			{
				animationEnded = true;
				return;
			}


		}
		//if the animation has ended don't continue
		if (animationEnded)
		{
			return;
		}

		//gets the key frame for the current robot part
		KeyFrame* currentKF = animController->keyFrames[animation][part.locationName][animationCurrentFrame];
		//gets the componenet for the current robot part
		HierarchicalComponent* currentHc = components[part.locationName];
		//smoothly lerps between different frames of the animation
		LerpRot(currentHc, currentKF, animation, part.locationName, startOfFrameTimer / endOfFrameTimer);
		LerpPos(currentHc, currentKF, animation, part.locationName, startOfFrameTimer / endOfFrameTimer);
	}
}

void Robot::LerpPos(HierarchicalComponent* currentHc, KeyFrame* currentKF, int animation, string partName, float time)
{
	//gets the frame start position
	XMVECTOR currentPos = XMLoadFloat4(&currentHc->GetStartPos());
	//gets the keyframe end position 
	XMVECTOR endPos = XMLoadFloat4(&currentKF->position);
	//performs the lerp
	XMVECTOR output = XMVectorLerp(currentPos, endPos, time);
	XMFLOAT4 lerpedFloat;
	XMStoreFloat4(&lerpedFloat, output);
	//sets this lerped location
	currentHc->SetPosition(lerpedFloat);
}
//	float time = startOfFrameTimer / endOfFrameTimer;

void Robot::LerpRot(HierarchicalComponent* currentHc, KeyFrame* currentKF, int animation, string partName, float time)
{
	XMVECTOR currentRot = XMLoadFloat4(&currentHc->GetStartRot());
	XMVECTOR endRot = XMLoadFloat4(&currentKF->rotation);
	XMVECTOR output = XMVectorLerp(currentRot, endRot, time);
	XMFLOAT4 lerpedFloat;
	XMStoreFloat4(&lerpedFloat, output);
	currentHc->SetRotation(lerpedFloat);
}

void Robot::TransitionAnimation(int animation)
{
	for each (RobotPart part in m_robotParts) 
	{
		//gets the key frame for the current robot part
		KeyFrame* currentKF = animController->keyFrames[animation][part.locationName][0];
		//gets the componenet for the current robot part
		HierarchicalComponent* currentHc = components[part.locationName];
		//lerps using the transition time
		float time = transitionTimer / transitionTime;
		LerpRot(currentHc, currentKF, animation, part.locationName, time);
		LerpPos(currentHc, currentKF, animation, part.locationName, time);


	}
}

void Robot::Die() 
{
	//if the robots not already dead
	if (currentAnimation != 1)
	{
		//kill the robot
		currentAnimation = 1;
		ResetGlobals(true);
	}
}





//a debug function to print all positions and rotations
void Robot::Debug(KeyFrame* currentKF, HierarchicalComponent* currentHc, RobotPart part) {
	dprintf("%s : pos(%f, %f ,%f), rot(%f ,%f ,%f )\n", part.locationName.c_str(), currentKF->position.x, currentKF->position.y, currentKF->position.z, currentKF->rotation.x, currentKF->rotation.y, currentKF->rotation.z);
	dprintf("%s : pos(%f, %f ,%f), rot(%f ,%f ,%f )\n\n", part.locationName.c_str(), currentHc->GetPosition().x, currentHc->GetPosition().y, currentHc->GetPosition().z, currentHc->GetRotation().x, currentHc->GetRotation().y, currentHc->GetRotation().z);
}
