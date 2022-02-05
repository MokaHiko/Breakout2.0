#include "pch.h"
#include "BallObject.h"

BallObject::BallObject()
	: GameObject(), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false) { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite) : 
	GameObject(pos, glm::vec2(radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true), Sticky(false), PassThrough(false){}

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
	Position = position;
	Velocity = velocity;
	Stuck = true;
	Sticky = false;
	PassThrough = false;
}
