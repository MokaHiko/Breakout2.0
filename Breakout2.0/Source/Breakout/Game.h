#pragma once
#include <GameLevel.h>
#include <PowerUp.h>
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	static Game& getInstance()
	{
		static Game instance;
		return instance;
	}

	Game(const Game&) = delete;
public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;

	void Init();

	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	inline bool isRunning() { return Running; }
	void DoCollision();

	void Clean();
public:
	std::vector<GameLevel> Levels;
	std::vector<PowerUp> PowerUps;
	unsigned int Level;
	
	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float dt);
	void ResetLevel();
private:
	Game();
	~Game();

	void InitResources();

	GLFWwindow* Window;
	bool Running = true;
private:
	//  GLFW Callbacks
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		// when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				getInstance().Keys[key] = true;

			else if (action == GLFW_RELEASE)
				getInstance().Keys[key] = false;
		}
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}
};

static Game& Core = Game::getInstance();
