#ifndef GAMETIME_H
#define GAMETIME_H

#include <Windows.h>

class GameTimer
{
public:
	//the delta time of the app
	static float deltaTime;

	static bool Initialize();
	static void Update();
private:
	//the start time
	static LONGLONG start;
	//ticks per second

	static float frequencySeconds;
};
#endif