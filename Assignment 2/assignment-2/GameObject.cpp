#include "GameObject.h"
#include "Sprite.h"

/*
* GameObject constructor
* @param sprite: Sprite object to be assigned to the GameObject
*/
GameObject::GameObject(Sprite* sprite) : sprite(sprite) {}


/*
* Adds a child GameObject to the GameObject
* @param child: GameObject to be added as a child
*/
void GameObject::addChild(GameObject* child) {
	children.push_back(child);
	child->parent = this;
}

/*
* Assigns this GameObject as a child to another GameObject
* @param parent: GameObject to add this GameObject as a child
*/
void GameObject::assignParent(GameObject* parent) {
	parent->addChild(this);
}

/*
* Updates the GameObject and its children
* @param dtime: Time since last frame
*/
void GameObject::update(float dtime) {
	sprite->position = getPos();
	sprite->rotation = global_rotation;
	sprite->scale = local_scale * getParentScale();

	for (auto child : children) {
		if (child != nullptr) {
			child->update(dtime);
		}
	}
}

/*
* Returns the global position of the GameObject
* @return: Position of the GameObject
*/
glm::vec2 GameObject::getPos() {
	if (parent == nullptr) {
		float angle = parent->self_rotation.GetRawValue();
		glm::vec2 pos = glm::vec2(
			relative_pos.x * glm::cos(angle) - relative_pos.y * glm::sin(angle),
			relative_pos.x * glm::sin(angle) + relative_pos.y * glm::cos(angle)
		);

		return pos * getParentScale() + parent->getPos();
	} else {
		return relative_pos;
	}
}

/*
* Returns the global rotation of the GameObject
* @return: Rotation of the GameObject
*/
Rotation GameObject::getRotation()
{
	return global_rotation;
}

/*
* Returns the radius of the GameObject
* @return: Radius of the GameObject
*/
float GameObject::getRadius() {
	return radius * local_scale * getParentScale();	
}

/*
* Returns the scale of the parent GameObject
* @return: Scale of the parent GameObject
*/
float GameObject::getParentScale() {
	if (parent == nullptr) {
		return 1.0f;
	}
	else {
		return parent->local_scale * parent->getParentScale();
	}
}

/*
* Sets the radius of the GameObject
* @param radius: Radius to set the GameObject to
*/
void GameObject::setRadius(float radius) {
	this->radius = radius;
}

/*
* Sets the relative position of the GameObject
* @param pos: Position to set the GameObject to
*/
void GameObject::setRelativePos(glm::vec2 pos) {
	relative_pos = pos;
}
