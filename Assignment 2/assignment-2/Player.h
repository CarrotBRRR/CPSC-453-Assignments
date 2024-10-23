#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
class Graphics;

class Player : public GameObject {
public:
	Player(Graphics graphics);

	virtual void update(float dtime);

	void mouseInput(float xpos, float ypos);
	void keyboardInput(float speed);

	bool moved();

	void scale(float s_factor);

private:
	Graphics graphics = nullptr;

	const float MAX_SPEED = 0.5f;
	const float DEAD_ZONE = 0.01f;

	float speed = 0.0f;
	bool moved_flag = false;
};

#endif
