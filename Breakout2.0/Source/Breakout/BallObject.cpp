#include "pch.h"
#include "BallObject.h"

BallObject::BallObject() {}

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite) : GameObject(pos, glm::vec2(radius * 2.0f), sprite)
{
	this->Velocity = velocity;
	this->Radius = radius;
	this->Stuck = true;
}

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
	if (Stuck) return  Position;
	Position += Velocity * dt;

	if (Position.x <= 0.0f)
	{
		Velocity.x = -Velocity.x;
		Position.x = 0.0f;
	}
	if (Position.x + Size.x >= window_width)
	{
		Velocity.x = -Velocity.x;
		Position.x = window_width - Size.x;
	}
	if (Position.y <= 0.0f)
	{
		Velocity.y = -Velocity.y;
		Position.y = 0.0f;
	}
	return Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{

}
