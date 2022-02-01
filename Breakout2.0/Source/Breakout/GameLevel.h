#pragma once
#include "GameObject.h"
class GameLevel
{
public:
	std::vector<GameObject> Bricks;
	
	GameLevel(){}

	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

	void Draw(SpriteRenderer& renderer);

	bool isComplete();
private:
	void init(std::vector<std::vector<unsigned int>> tileData,
		unsigned int levelWidth, unsigned int levelHeight);
};

