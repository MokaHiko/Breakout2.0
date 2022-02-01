#include "pch.h"
#include "Game.h"
int main()
{
	float dt = 0;
	float curTime = 0;
	float lastTime = 0;

	Core.Init(); 
	while (Core.isRunning())
	{
		curTime = glfwGetTime(); 
		dt = curTime - lastTime;
		lastTime = curTime;

		Core.ProcessInput(dt);
		Core.Update(dt);
		Core.Render();
	}

	Core.Clean();
	return 0;
}

