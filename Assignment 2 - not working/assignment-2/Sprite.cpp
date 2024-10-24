#include "Sprite.h"
#include "Graphics.h"

Sprite::Sprite(int texture_id, Graphics* graphics) {
	if (graphics == nullptr) {
		return;
	}

	graphics->addSprite(this);
	this->texture_id = texture_id;
}
