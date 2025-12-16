#pragma once

#include <Windows.h>
#include <iostream>

using Count = long long;

Count g_Frequency = 0;
Count g_LastTime = 0;

void InitializeQPCTimer()
{
	LARGE_INTEGER li;
	if(QueryPerformanceFrequency(&li))
	{
		g_Frequency = li.QuadPart;

		QueryPerformanceCounter(&li);
		g_LastTime = li.QuadPart;
	}
	else
	{
		g_Frequency = 0;
	}
}

float Tick()
{
	if(g_Frequency == 0)
	{
		return 0.0f;
	}

	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);
	Count currentTime = li.QuadPart;

	// Calculate the difference (delta time)
	Count deltaTime = currentTime - g_LastTime;

	g_LastTime = currentTime;


	double secondsPerCount = 1.0 / (double)g_Frequency;
	float dt = (float)((double)deltaTime * secondsPerCount);
	
	return dt;
}

float PeekDeltaTime()
{
	if (g_Frequency == 0) return 0.0f;

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	Count currentTime = li.QuadPart;

	// Calculate difference but DO NOT update g_LastTime yet
	Count elapsedTicks = currentTime - g_LastTime;

	double secondsPerCount = 1.0 / (double)g_Frequency;

	// Return the result as a float
	return (float)((double)elapsedTicks * secondsPerCount);
}