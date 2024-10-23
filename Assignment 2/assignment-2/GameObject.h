#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Rotation.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sprite;

class GameObject {
public:
	GameObject(Sprite* sprite);

	void addChild(GameObject* child);
	void assignParent(GameObject* parent);

	void update(float dtime);

	glm::vec2 getPos();
	Rotation getRotation();
	float getRadius();

	float getParentScale();

	void setRadius(float radius);
	void setRelativePos(glm::vec2 pos);

protected:
	Sprite* sprite = nullptr;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	glm::vec2 relative_pos = glm::vec2(0.f, 0.f);

	Rotation global_rotation = 0.0f;
	Rotation self_rotation = 0.0f;

	float local_scale = 1.f;
	float child_scale = 1.f;

	float radius = 0.0f;
};

#endif
