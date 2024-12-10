#pragma once
#include "GameObject.h"
#include "Graphics.h"

class Player;
class Graphics;

#define ORBIT_RADIUS 0.15f
#define ORBIT_SPEED 2.f
#define COLLECT_DIST 0.2f

class Pickup : public GameObject {
public:
	Pickup(float speed, float direction, glm::vec2 pos, Graphics* graphics);

	virtual void update(float dtime);

	int playerCollision(glm::vec2 player_pos, float player_radius);

	void bindPlayer(Player* player, int score);

private:
	std::shared_ptr<GameObject> fire_obj = nullptr;

	float speed = 0.f;
	glm::vec2 direction = glm::vec2(0.f, 0.f);

	std::unique_ptr<Graphics> graphics = nullptr;

	bool collected = false;

	glm::vec2 clampPos(glm::vec2 pos);
};
