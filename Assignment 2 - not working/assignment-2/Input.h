#pragma once

#include <memory>

class Player;

class Input {
public:
	void setPlayer(Player* player);

	void keyboardInput(int key, int scancode, int action, int mods);

	void mouseInput(double x, double y);

	void windowSizeInput(int width, int height);

private:
	int WINDOW_WIDTH = 800;
	int WINDOW_HEIGHT = 600;

	Player* player = nullptr;
};
