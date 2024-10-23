#ifndef INPUT_H
#define INPUT_H

class Player;

class Input {
public:
	void setPlayer(Player* player);

	void keyboardInput(int key, int scancode, int action, int mods);

	void mouseInput(double x, double y);

private:
	Player* player = nullptr;

	int WINDOW_HEIGHT = 800;
	int WINDOW_WIDTH = 800;
};

#endif 
