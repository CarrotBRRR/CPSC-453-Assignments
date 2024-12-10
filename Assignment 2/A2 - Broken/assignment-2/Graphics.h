#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "Window.h"

class Window;
class ShaderProgram;
class Texture;
class Sprite;
class Input;

class Graphics {
public:
	Graphics(int n_sprites);

	bool init(Input* input_manager);

	bool shouldClose();

	void win();

	void updateScore(int score);

	void updateGraphics();

	void addSprite(Sprite* sprite);

	void stop();

	void resetGraphics();

private:
	const glm::vec2 SCR_DIM = { 800, 800 };

	unsigned int TBO; // transform buffer object id
	unsigned int IBO; // texture id buffer object id
	unsigned int VBO; // vertex buffer object
	unsigned int EBO; // element buffer object
	unsigned int VAO; // vertex array object

	float verts[20] = {
		// positions xyz     // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left
	};

	unsigned int indices[6] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	std::vector<Sprite*> sprites;

	int sprite_count = 0; // Number of added sprites
	int n_sprites = 0; // Number of sprites

	std::vector<glm::mat4> transforms; // sprite transforms
	std::vector<glm::int32> texture_ids; // sprite texture ids

	std::unique_ptr<Window> window = nullptr;
	ShaderProgram* shader_program = nullptr;
	Input* input_manager = nullptr;

	Texture* player_ship_tex = nullptr;
	Texture* diamond_tex = nullptr;
	Texture* fire_tex = nullptr;

	bool win_state = false;
	int score = 0;

	void sendData2GPU();

	void updateSprites();

	void updateGUI();
};
