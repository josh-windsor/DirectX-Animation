#include "GameTimer.h"

float GameTimer::deltaTime = 0;
float GameTimer::frequencySeconds = 0;
LONGLONG GameTimer::start = 0;



bool GameTimer::Initialize()
{
	LARGE_INTEGER i;

	//calls the query once at the start
	if (!QueryPerformanceFrequency(&i))
		return false;

	//ticks per second
	frequencySeconds = (float)(i.QuadPart);

	//querys the timer
	QueryPerformanceCounter(&i);
	//sets and initilizes the start variables
	start = i.QuadPart;

	return true;
}

void GameTimer::Update()
{
	LARGE_INTEGER i;
	//gets the time since last frame
	QueryPerformanceCounter(&i);
	//sets the delta time
	deltaTime = (float)(i.QuadPart - start) / frequencySeconds;

	start = i.QuadPart;
}

