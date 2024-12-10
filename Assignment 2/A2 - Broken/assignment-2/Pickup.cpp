#define _USE_MATH_DEFINES

#include "Pickup.h"
#include "Sprite.h"
#include "Rotation.h"
#include "Player.h"

#include <memory>
#include <cmath>

Pickup::Pickup(float speed, float direction, glm::vec2 pos, Graphics* graphics) : GameObject(nullptr) {
	shared_sprite = std::make_shared<Sprite>(DIAMOND_TEX_ID, graphics);
	sprite = shared_sprite.get();
	sprite->sprite_dims = glm::vec2(0.15f, 0.15f);
	radius = 0.075f;

	std::shared_ptr<Sprite> fire = std::make_shared<Sprite>(FIRE_TEX_ID, graphics);
	fire->sprite_dims = glm::vec2(0.1f, 0.1f);
	fire_obj = std::make_shared<GameObject>(fire.get());
	fire_obj->setRadius(0.05f);

	addChild(fire_obj.get());
	fire_obj->setRelativePos(glm::vec2(ORBIT_RADIUS, 0.0f));

	this->speed = speed;
	this->direction = glm::vec2(cos(direction), sin(direction));
	this->relative_pos = pos;
}

void Pickup::update(float dtime) {
	if (!collected) {
		relative_pos += direction * speed * dtime;

		relative_pos = clampPos(relative_pos);
	} else if (parent != nullptr) {
		global_rotation = parent->getRotation() - M_PI_2;
	}

	self_rotation += ORBIT_SPEED * dtime;

	sprite->position = getPos();
	sprite->rotation = global_rotation;
	sprite->scale = local_scale * getParentScale();

	for (auto child : children) {
		if (child != nullptr) {
			child->update(dtime);
		}
	}
}

glm::vec2 Pickup::clampPos(glm::vec2 pos) {
	if (pos.x > 1.0f) {
		pos.x = 1.0f;
	}
	else if (pos.x < -1.0f) {
		pos.x = -1.0f;
	}

	if (pos.y > 1.0f) {
		pos.y = 1.0f;
	}
	else if (pos.y < -1.0f) {
		pos.y = -1.0f;
	}

	return pos;
}

int Pickup::playerCollision(glm::vec2 player_pos, float player_radius) {
	if (collected) {
		return 0;
	}

	glm::vec2 diff = player_pos - fire_obj->getPos();
	float dist = glm::length(diff);

	float touch_radius = player_radius + fire_obj->getRadius();

	if (dist < touch_radius) {
		return 2;
	}

	diff = player_pos - getPos();
	dist = glm::length(diff);

	touch_radius = player_radius + getRadius();

	if (dist < touch_radius) {
		return 1;
	}

	return 0;
}

void Pickup::bindPlayer(Player* player, int score) {
	collected = true;

	assignParent(player);

	relative_pos = glm::vec2(0.0f, -COLLECT_DIST * (float)score - 0.1f);
}
