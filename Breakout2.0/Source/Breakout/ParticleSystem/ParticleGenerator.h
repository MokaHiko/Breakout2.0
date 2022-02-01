#pragma once
#include "GameObject.h"
#include "Shader.h"
#include "Texture.h"

struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle()
		:Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int nParticles);

	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset);
	void Draw();
private:
	void init();

	unsigned int nr_particles;
	std::vector<Particle> particles;

	unsigned int firstUnusedParticle();
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset);
private:
	unsigned int VAO;

	Texture2D texture;
	Shader shader;
};

