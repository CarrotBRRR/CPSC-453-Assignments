#include "Input.h"
#include "Player.h"

void Input::setPlayer(Player* player) {
	this->player = player;
}

void Input::keyboardInput(int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		player->moveInput(0.0f);

	} else if (action == GLFW_PRESS || action == GLFW_REPEAT) {

		if (key == GLFW_KEY_W) {
			player->moveInput(1.0f);

		} else if (key == GLFW_KEY_S) {
			player->moveInput(-1.0f);
		}

	}
}

void Input::mouseInput(double x, double y) {
	float x_pos = (x * 2.0f / (float)WINDOW_WIDTH) - 1.0f;
	float y_pos = (y * 2.0f / (float)WINDOW_HEIGHT) - 1.0f;

	player->steerInput(x_pos, -y_pos);
}
