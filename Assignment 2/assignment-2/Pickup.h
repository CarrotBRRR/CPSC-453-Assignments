#ifndef PICKUP_H
#define PICKUP_H

#include "GameObject.h"

class Player;
class Graphics;

class Pickup : public GameObject {
public:
	Pickup(float speed, float direction, glm::vec2 pos, Graphics* graphics);

	virtual void update(float dtime);

	int playerCollision(glm::vec2 player_pos, float player_radius);

	void bindPlayer(Player* player, int score);

private:
	const float ORBIT_R = 0.15f; // Orbit radius
	const float ORBIT_SPEED = 2.f; // Orbit speed
	const float BOUND_DIST = 0.2f; // Spacing when collected

	GameObject* fire = nullptr;

	float speed = 0.f;
	glm::vec2 direction = glm::vec2(0.f, 0.f);

	Graphics* graphics = nullptr;

	bool collected = false;
};

#endif
