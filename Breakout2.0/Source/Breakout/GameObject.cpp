#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
	:Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Sprite(sprite), Color(color), Velocity(velocity), IsSolid(false), Rotation(0.0f), Destroyed(false) {}

void GameObject::Draw(SpriteRenderer& renderer)
{
	renderer.DrawSprite(Sprite, Position, Size, Rotation, Color);
}
