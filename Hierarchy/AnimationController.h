#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H
#include "Application.h"

#include "tinyxml2.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>    // std::mismatch

using namespace std;

//stores the passed xml data
struct AnimationSegment
{
	string m_sourceId;
	vector<double> m_inputArr;
	vector<double> m_outputArr;
};

//stores the keyframe values
struct KeyFrame
{
	XMFLOAT4 position;
	XMFLOAT4 rotation;
};


class AnimationController
{
public:
	AnimationController();
	~AnimationController();
	//stores animation - > location -> framenumber -> translation
	vector<map<string, vector<KeyFrame*>>> keyFrames;
	//stores animation - > location -> framenumber -> animation time
	vector<map<string, vector<double>>> animTimings;


private:
	//string to ignore on the xml passing
	const string toFind = "visibility";

	void PadValues();

	void ConvertDataToKeyframe();


	void ProcessElement(tinyxml2::XMLElement* element, int animation);
	vector<double> PullDoubles(string inputArr);
	void LoadAnimation(string fileURL);


	vector<vector<AnimationSegment>> m_AnimSegments;

};

#endif