#pragma once

#include "GameObject.h"
#include "Graphics.h"

class Graphics;

#define DEAD_ZONE 0.01f

class Player : public GameObject {
public:
	Player(Graphics* graphics);

	virtual void update(float dtime);

	void steerInput(float xpos, float ypos);
	void moveInput(float speed);

	bool moved();

	void scale(float s_factor);

private:
	Graphics* graphics = nullptr;

	float speed = 0.0f;
	bool moved_flag = false;

	glm::vec2 clampPos(glm::vec2 pos);
};
