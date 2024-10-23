#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

class Player;

class Input {
public:
	void setPlayer(Player* player);

	void keyboardInput(int key, int scancode, int action, int mods);

	void mouseInput(double x, double y);

private:
	Player* player = nullptr;

	int 
	int 
};

#endif 
