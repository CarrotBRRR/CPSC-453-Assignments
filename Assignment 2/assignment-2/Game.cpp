#include "Game.h"
#include "Player.h"
#include "Input.h"
#include "Graphics.h"
#include "Pickup.h"

#include <chrono>
#include <iostream>
#include <memory>

/*
* Game constructor
* Initializes the Game object
* @param n_pickups: Number of pickups to be spawned
*/
Game::Game(int n_pickups) : n_pickups(n_pickups) {}


/*
* Initializes the game
*/
void Game::initGame() {
	score = 0;
	graphics = std::make_unique<Graphics>((n_pickups * 2) + 1); // n_pickups, *2 for fire, +1 player

	player = std::make_unique<Player>(graphics);

	Input* input_manager = std::make_unique<Input>();
	input_manager->setPlayer(player);

	pickups.resize(n_pickups);
	for (int i = 0; i < n_pickups; i++) {
		float speed = (((float)(rand() % 1000)) * 0.001f) * (MAX_DIAMOND_SPEED - MIN_DIAMOND_SPEED) + MIN_DIAMOND_SPEED;
		float rotation = (((float)(rand() % 1000)) * 0.006f);

		float x = (((float)(rand() % 1000)) * 0.001f) * 2.0f - 1.0f;
		float y = (((float)(rand() % 1000)) * 0.001f) * 2.0f - 1.0f;

		Pickup* pickup = std::shared_ptr<Pickup>(new Pickup(x, y, speed, rotation, graphics));
		pickups[i] = pickup;
	}

	graphics->init(input_manager);
}

/*
* Main game loop
*/
void Game::gameLoop(){
	game_over = false;

	std::chrono::high_resolution_clock clock;
	auto time = std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count();
	auto last_time = time;

	int64_t d_time = 0;
	float d_time_seconds = 0.0f;

	while (!graphics->shouldClose()) {

		time = std::chrono::duration_cast<std::chrono::microseconds>(clock.now().time_since_epoch()).count();
		d_time = time - last_time;
		d_time_seconds = d_time / 1000000.0f;
		last_time = time;

		if (game_over) {
			game_over = false;
			resetGame();
			d_time = 0;
		}

		updateGame(d_time_seconds);
	}

	graphics->stop();
}

/*
* Starts the game
*/
void Game::startGame() {
	initGame();
	gameLoop();
}

/*
* Updates the game
* @param dtime: Time since last frame
*/
void Game::updateGame(float dtime) {
	player->update(dtime);

	for (int i = 0; i < n_pickups; i++) {
		pickups[i]->update(dtime);

		if (!ship->moved()) {
			continue;
		}

		switch (pickups[i]->playerCollision(player->getPos(), player->getRadius())) {
		case 0: // no collision
			break;

		case 1: // diamond
			score++;
			pickups[i]->bindPlayer(player, score);
			player->scale(1.1f);
			graphics->updateScore(score);

			if (score == n_pickups) {
				graphics->win();
			}

			break;

		case 2: // fire
			game_over = true;
			break;
		}		
	}

	graphics->updateGraphics();
}

/*
* Resets the game
*/
void Game::resetGame() {
	graphics->resetGraphics();

	player = std::make_unique<Player>(graphics);
	input_manager->setPlayer(player);

	score = 0;

	for (int i = 0; i < n_pickups; i++) {
		float speed = (((float)(rand() % 1000)) * 0.001f) * (MAX_DIAMOND_SPEED - MIN_DIAMOND_SPEED) + MIN_DIAMOND_SPEED;
		float rotation = (((float)(rand() % 1000)) * 0.006f);

		float x = (((float)(rand() % 1000)) * 0.001f) * 2.0f - 1.0f;
		float y = (((float)(rand() % 1000)) * 0.001f) * 2.0f - 1.0f;

		Pickup* pickup = std::shared_ptr<Pickup>(new Pickup(x, y, speed, rotation, graphics));
		pickups[i] = pickup;
	}
}
