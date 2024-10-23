#ifndef SPRITE_H
#define SPRITE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Rotation.h"

class Graphics;

class Sprite {
public:
	Sprite(int texture_id, Graphics* graphics);

	glm::vec2 position = glm::vec2(0.f, 0.f);
	glm::vec2 sprite_dims = glm::vec2(0.15, 0.15);

	float scale = 1.f;

	Rotation rotation = 0.0f;

	int texture_id = 0;

	Graphics* graphics = nullptr;

};

