#include "Graphics.h"
#include "Window.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Sprite.h"
#include "Input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Graphics::Graphics(int n_sprites) {
	// Initialize the sprite vector with n_sprites
	sprites.resize(n_sprites);
	this->n_sprites = n_sprites;

	sprite_count = 0;
}

bool Graphics::init(Input* manager) {
	sprite_count = 0;
	transforms.resize(n_sprites);
	texture_ids.resize(n_sprites);

	window = std::make_unique<Window>(WIDTH, HEIGHT, "Assignment 2");
	if (!window->init()) {
		std::cerr << "Failed to initialize window" << std::endl;
		return false;
	}

	window->setInputManager(manager);
	input_manager = manager;
	input_manager->setWindow(SCR_DIM.x, SCR_DIM.y);

	shader_program = std::make_unique<ShaderProgram>("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

	fire_tex = std::make_unique<Texture>("textures/fire.png");
	diamond_tex = std::make_unique<Texture>("textures/diamond.png");
	player_ship_tex = std::make_unique<Texture>("textures/player_ship.png");

	// imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// Transform buffer object
	glGenBuffers(1, &TBO);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * num_sprites, sprite_transforms.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Texture id buffer object
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::int32) * num_sprites, texture_ids.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &VAO); // vertex array object, allocate memory for an array of values
	glGenBuffers(1, &VBO);      // vertex buffer object, the buffer to place values in
	glGenBuffers(1, &EBO);	    // element buffer object, the buffer to place elements in

	// bind the Vertex Array Object first
	glBindVertexArray(VAO);

	// bind and set vertex buffer(s)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// bind and set element buffer(s)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // this is to a element_array_buffer so it does not interfer with the array_buffer data being setup

	// place the positions into position 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // this is the position of the first value in the array

	// place the texture coords into position 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); 

	// bind the texture id buffer object
	glBindBuffer(GL_ARRAY_BUFFER, IBO);

	// place the texture ids in position 2
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(glm::int32), (void*)0);

	// make the texture ids change instances not vertices or fragments
	glVertexAttribDivisor(2, 1);

	// bind the transform buffer object
	glBindBuffer(GL_ARRAY_BUFFER, TBO);

	// place the transforms in position 3 to 6
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	// make the transforms change instances not vertices or fragments
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader_program->use();
	glUniform1i(glGetUniformLocation(shader_program->getID(), "texture1"), 0); // set the position of the first  texture to 0 (TEXTURE0)
	glUniform1i(glGetUniformLocation(shader_program->getID(), "texture2"), 1); // set the position of the second texture to 1 (TEXTURE1)
	glUniform1i(glGetUniformLocation(shader_program->getID(), "texture3"), 2); // set the position of the third  texture to 2 (TEXTURE2)

	glActiveTexture(GL_TEXTURE0);
	fire_tex->bind();
	glActiveTexture(GL_TEXTURE1);
	diamond_tex->bind();
	glActiveTexture(GL_TEXTURE2);
	player_ship_tex->bind();

	glBindVertexArray(VAO);

	return true;
}

bool Graphics::shouldClose() {
	return window->shouldClose();
}

void Graphics::win() {
	win = true;
}

void Graphics::updateScore(int score) {
	this->score = score;
}

void Graphics::updateGraphics() {
	glfwPollEvents();

	updateSpriteData();

	sendData2GPU();

	glEnable(GL_FRAMEBUFFER_SRGB);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shoutout to my friend for helping me find this way to only need a single draw call
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, sprite_count);
	glDisable(GL_FRAMEBUFFER_SRGB);

	updateGUI();

	glfwSwapBuffers(window->getWindow());

	// check to see if escape key has been pressed
	if (glfwGetKey(window->GetWindowPointer(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window->GetWindowPointer(), true);
	}

	// cursor data
	double xpos, ypos;
	glfwGetCursorPos(window->GetWindowPointer(), &xpos, &ypos);
	input_manager->mouseInput(xpos, ypos);
}

void Graphics::addSprite(Sprite* sprite) {
	if (sprite_count < n_sprites) {
		sprites[sprite_count] = sprite;
		sprite_count++;
	}
}

void Graphics::stop() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &TBO);
	glDeleteBuffers(1, &IBO);

	shader_program->deleteProgram();
	glfwTerminate();
}

void Graphics::resetGraphics() {
	sprite_count = 0;
	score = 0;
	win = false;
}

void Graphics::updateSprites() {
	for (int i = 0; i < sprite_count; i++) {
		Sprite* sprite = sprites[i];
		glm::mat4 transformer = glm::mat4(1.0f); // robots in disguise

		// position transform
		transformer = glm::translate(transformer, glm::vec3(sprite->position.x, sprite->position.y, 0.0f));
		// rotation transform
		transformer = glm::rotate(transformer, sprite->rotation.GetRawValue(), glm::vec3(0.0f, 0.0f, 1.0f));
		// scale transform
		transformer = glm::scale(transformer, glm::vec3(sprite->sprite_dims.x * sprite->scale, sprite->sprite_dims.y * sprite->scale, 1.0f));

		transforms[i] = transformer;
		texture_ids[i] = sprite->texture_id;
	}
}

void Graphics::sendData2GPU() {
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * num_sprites, sprite_transforms.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::int32) * num_sprites, texture_ids.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Graphics::updateGUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(5, 5));

	ImGuiWindowFlags textWindowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("Score", (bool*)0, textWindowFlags);
	ImGui::SetWindowFontScale(3.f);
	if (win) {
		ImGui::Text("Congratulations! You have won!");

	} else {
		ImGui::Text("Score: %d", score);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



	

