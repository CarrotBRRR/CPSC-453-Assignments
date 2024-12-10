#include "Input.h"
#include "Player.h"

#include <GLFW/glfw3.h>

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
	float x_pos = ((float)x * 2.0f / (float)WINDOW_WIDTH) - 1.0f;
	float y_pos = ((float)y * 2.0f / (float)WINDOW_HEIGHT) - 1.0f;

	player->steerInput(x_pos, -y_pos);
}

void Input::windowSizeInput(int width, int height) {
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}
