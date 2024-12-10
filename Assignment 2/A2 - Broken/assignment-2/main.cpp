#define GLFW_INCLUDE_NONE

#include "Game.h"

int main() {
	Game game = Game(8);
	game.startGame();

	return 0;
}
