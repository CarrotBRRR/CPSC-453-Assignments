#pragma once

#include <vector>
#include <memory>

#include "Graphics.h"

class Player;
class Input;
class Graphics;
class Pickup;

class Game {
public:
	Game(int n_pickups);
	~Game();

	void startGame();

private:
	void gameLoop();

	void initGame();
	void updateGame(float dtime);
	void resetGame();

	const float MAX_DIAMOND_SPEED = 0.5f;
	const float MIN_DIAMOND_SPEED = 0.1f;

	int n_pickups; // number of diamonds

	Player* player = nullptr;
	Input* input_manager = nullptr;
	Graphics* graphics;

	int score = 0;

	std::vector<Pickup*> pickups;

	bool game_over = false;
};
