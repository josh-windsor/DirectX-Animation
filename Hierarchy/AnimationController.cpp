#include "AnimationController.h"

using namespace std;



AnimationController::AnimationController()
{
	//loads each animation
	LoadAnimation("Resources/Robot/RobotIdleAnim.dae");
	LoadAnimation("Resources/Robot/RobotDieAnim.dae");
	LoadAnimation("Resources/Robot/RobotAttackAnim.dae");

	//Additional animations could be loaded here//


	PadValues();

	ConvertDataToKeyframe();
}

void AnimationController::PadValues()
{
	//loops through all animations
	for (int i = 0; i < m_AnimSegments.size(); i++)
	{
		//detemines the largest size of the array
		int maxSize = 0;
		for (size_t j = 0; j < m_AnimSegments[i].size(); j++)
		{
			if (m_AnimSegments[i][j].m_inputArr.size() > maxSize)
			{
				maxSize = m_AnimSegments[i][j].m_inputArr.size();
			}
		}
	
		for (size_t j = 0; j < m_AnimSegments[i].size(); j++)
		{
			int iterator = 0;
			//loops through padding the translations with their x y and z values repeated
			while (m_AnimSegments[i][j].m_inputArr.size() < maxSize)
			{
				m_AnimSegments[i][j].m_inputArr.push_back(m_AnimSegments[i][j].m_inputArr[iterator]);
				m_AnimSegments[i][j].m_outputArr.push_back(m_AnimSegments[i][j].m_outputArr[0]);
				m_AnimSegments[i][j].m_outputArr.push_back(m_AnimSegments[i][j].m_outputArr[1]);
				m_AnimSegments[i][j].m_outputArr.push_back(m_AnimSegments[i][j].m_outputArr[2]);
				iterator++;
			}
		}
	}
}

void AnimationController::ConvertDataToKeyframe() {
	for (int i = 0; i < m_AnimSegments.size(); i++)
	{
		//adds a new segment for each animation
		keyFrames.push_back(map<string, vector<KeyFrame*>>());
		animTimings.push_back(map<string, vector<double>>());

		//loops through all animation segments
		for (int animSegI = 0; animSegI < m_AnimSegments[i].size(); animSegI++)
		{
			AnimationSegment segment = m_AnimSegments[i][animSegI];
			string name = segment.m_sourceId;
			string lName, rName;
			string* p_str = &lName;
			//loops through all names
			for each (char c in name)
			{
				if (c == '.')
				{
					//moves the pointer to add to the right name
					p_str = &rName;
				}
				else
				{
					*p_str += c;
				}
			}
			int f = 0;
			//resizes the arrays to the size of the animation
			keyFrames[i][lName].resize(segment.m_inputArr.size());
			animTimings[i][lName].resize(segment.m_inputArr.size());

			//loops through the input array size
			for (int j = 0; j < segment.m_inputArr.size(); j++)
			{
				KeyFrame* currentKeyFrame;

				//detemines if there is already an entry in the map
				if (keyFrames[i].find(lName) == keyFrames[i].end())
				{
					//not found - creates a new entry
					currentKeyFrame = new KeyFrame();
				}
				else
				{
					//if the keyframe is null
					if (keyFrames[i][lName][j] == nullptr)
					{
						//create a new keyframe in the array
						keyFrames[i][lName][j] = new KeyFrame();
					}
					//if found, get the keyframe
					currentKeyFrame = keyFrames[i][lName][j];
				}
				//if the name is translate
				if (rName == "translate")
				{
					//sets the keyframe with the translation data divided by 10
					currentKeyFrame->position.x = segment.m_outputArr[f] / 10;
					f++;
					currentKeyFrame->position.y = segment.m_outputArr[f] / 10;
					f++;
					currentKeyFrame->position.z = segment.m_outputArr[f] / 10;
					f++;

				}
				//if its rotate
				else if (rName.find("rotate") == 0)
				{
					//add to the appropriate x y or z location
					switch (rName.back())
					{
					case 'X':
						currentKeyFrame->rotation.x = segment.m_outputArr[j];
						break;
					case 'Y':
						currentKeyFrame->rotation.y = segment.m_outputArr[j];
						break;
					case 'Z':
						currentKeyFrame->rotation.z = segment.m_outputArr[j];
						break;
					default:
						break;
					}
				}
				//sets the input array
				animTimings[i][lName] = segment.m_inputArr;
				//sets the current key frame
				keyFrames[i][lName][j] = currentKeyFrame;
				//makes sure that the keyframe is not used again accidently
				currentKeyFrame = nullptr;
			}
		}
	}
}



AnimationController::~AnimationController()
{
	
}

void AnimationController::LoadAnimation(string fileURL) 
{
	//loads xml from the passed in file url
	tinyxml2::XMLDocument doc;
	doc.LoadFile(fileURL.c_str());
	//gets to the correct starting point
	tinyxml2::XMLElement* title = doc.FirstChildElement("COLLADA")->FirstChildElement("library_animations")->FirstChild()->ToElement();
	
	m_AnimSegments.push_back(vector<AnimationSegment>());
	//recursively fills the animations array
	ProcessElement(title, m_AnimSegments.size() - 1);
}

void AnimationController::ProcessElement(tinyxml2::XMLElement* element, int animation) 
{
	//gets the id attribute
	string elementId = element->Attribute("id");
	//if the name is visibility
	if (std::mismatch(toFind.rbegin(), toFind.rend(), elementId.rbegin()).first == toFind.rend())
	{
		if (element->NextSibling() != nullptr)
		{
			//skip to the next sibling, ignoring visibility
			element = element->NextSibling()->ToElement();
		}
		else
		{
			return;
		}
	}

	//creates a new segment
	AnimationSegment segment;

	//sets the source id of the animation segment
	segment.m_sourceId = element->Attribute("id");

	//gets the source element
	tinyxml2::XMLElement* inputElement = element->FirstChildElement("source");
	
	//gets the input and output variables
	segment.m_inputArr = PullDoubles(inputElement->FirstChild()->ToElement()->GetText());
	segment.m_outputArr = PullDoubles(inputElement->NextSibling()->FirstChild()->ToElement()->GetText());
	
	//adds the animation segment to the vector
	m_AnimSegments[animation].push_back(segment);

	//if there is still a next sibling
	if (element->NextSibling() != nullptr)
	{
		//recursively process the next element
		ProcessElement(element->NextSibling()->ToElement(), animation);
	}
}

vector<double> AnimationController::PullDoubles(string inputArr) 
{
	vector<string> inputStrArr;
	inputStrArr.resize(1);
	int k = 0;
	//loops through all characters
	for (char& c : inputArr) {
		//if the space delimited is present
		if (c == ' ')
		{
			//move to the next segment of the array
			k++;
			inputStrArr.push_back("");
			continue;
		}
		inputStrArr[k] += c;
	}
	vector<double> outArr;
	for each (string number in inputStrArr)
	{
		//converts the array to a float and adds to the output
		outArr.push_back(atof( number.c_str()));
	}
	return outArr;
}

