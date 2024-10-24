#define _USE_MATH_DEFINES

#include "Player.h"
#include "Sprite.h"

#include <memory>
#include <cmath>
#include <iostream>

Player::Player(Graphics* graphics) : GameObject(nullptr) {
	// Check if graphics is valid before creating the sprite
	if (graphics != nullptr) {
		sprite = new Sprite(PLAYER_TEX_ID, graphics);
		sprite->sprite_dims = glm::vec2(0.175f, 0.125f);
	}
	else {
		std::cerr << "Graphics pointer is null in Player constructor!" << std::endl;
		sprite = nullptr;
	}

	radius = 0.05f;
	local_scale = 0.5f;
}

void Player::update(float dtime) {
	if (sprite != nullptr) {
		relative_pos = relative_pos + glm::vec2(-glm::sin(global_rotation.getValue()), glm::cos(global_rotation.getValue())) * speed * dtime;
		sprite->position = getPos();
		sprite->rotation = global_rotation;
		sprite->scale = local_scale * getParentScale();
	}
	else {
		std::cerr << "Sprite pointer is null in update!" << std::endl;
	}

	relative_pos = clampPos(relative_pos);

	for (auto& child : children) {
		child->update(dtime);
	}
}

glm::vec2 Player::clampPos(glm::vec2 pos) {
	if (pos.x < -1.0f) {
		pos.x = -1.0f;

	} else if (pos.x > 1.0f) {
		pos.x = 1.0f;
	}

	if (pos.y < -1.0f) {
		pos.y = -1.0f;

	} else if (pos.y > 1.0f) {
		pos.y = 1.0f;
	}

	return pos;
}

void Player::steerInput(float xpos, float ypos) {
	if (sprite != nullptr) {
		float dy = ypos - sprite->position.y;
		float dx = xpos - sprite->position.x;

		float dist_mouse = ((ypos * ypos) + (xpos * xpos) - DEAD_ZONE) * DEAD_ZONE;
		if (dist_mouse < 0.0f) {
			speed = 0.0f;
			return;
		}

		global_rotation = std::atan2(dy, dx) - M_PI_2;
		self_rotation = global_rotation;
	}
	else {
		std::cerr << "Sprite pointer is null in steerInput!" << std::endl;
	}
}

void Player::moveInput(float speed) {
	this->speed = speed;

	if (abs(speed) > 0.9f && moved_flag == false) {
		moved_flag = true;
	}
}

bool Player::moved() {
	return moved_flag;
}

void Player::scale(float s_factor) {
	local_scale *= s_factor;
}
