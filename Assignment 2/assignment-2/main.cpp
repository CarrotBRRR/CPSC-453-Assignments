#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "Callbacks.h"
#include "Game.h"

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	Window window(800, 800, "CPSC 453: Assignment 2"); // can set callbacks at construction if desired

	GLDebug::enable();

	// SHADERS
	ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

	// CALLBACKS
	std::shared_ptr<MyCallbacks> callback = std::make_shared<MyCallbacks>(shader);
	window.setCallbacks(callback);

	GameObject ship("textures/ship.png", GL_NEAREST);
	GameObject diamond1("textures/diamond.png", GL_NEAREST, glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f), glm::vec3(0.5f,0.25f,0.f));
	GameObject diamond2("textures/diamond.png", GL_NEAREST, glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f), glm::vec3(0.25f,0.5f,0.f));
	GameObject diamond3("textures/diamond.png", GL_NEAREST, glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f), glm::vec3(-0.5f,-0.5f,0.f));
	GameObject diamond4("textures/diamond.png", GL_NEAREST, glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f), glm::vec3(-0.15f,0.35f,0.f));
	GameObject diamond5("textures/diamond.png", GL_NEAREST, glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f), glm::vec3(0.5f,-0.5f,0.f));

	std::vector<GameObject*> uncollected_dia_base = std::vector<GameObject*>();
	uncollected_dia_base.push_back(&diamond1);
	uncollected_dia_base.push_back(&diamond2);
	uncollected_dia_base.push_back(&diamond3);
	uncollected_dia_base.push_back(&diamond4);
	uncollected_dia_base.push_back(&diamond5);
	
	std::vector<GameObject*> uncollected_dia = std::vector<GameObject*>();
	uncollected_dia.push_back(&diamond1);
	uncollected_dia.push_back(&diamond2);
	uncollected_dia.push_back(&diamond3);
	uncollected_dia.push_back(&diamond4);
	uncollected_dia.push_back(&diamond5);

	int score = 0;
	bool start = false;
	int maxScore = uncollected_dia.size();

	// RENDER LOOP
	while (!window.shouldClose()) {
		
		Params user_input = callback->getParams();

		if (user_input.resetFlag){
			resetGame(&ship, &uncollected_dia_base, &uncollected_dia);
			score = 0;
			start = false;
		}

		if (user_input.move_distance != glm::vec3(0.f,0.f,0.f)){
			start = true;
		}

		glfwPollEvents();

		shader.use();

		// Clear the screen
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::vector<int> collectedIndices = std::vector<int>();
		int newlyCollected = 0;
		// Then render the diamonds
		for (int i = 0; i < int(uncollected_dia.size()); i++){

			auto curr = uncollected_dia.at(i);

			(*curr).ggeom.bind();
			(*curr).texture.bind();

			(*curr).updateDiamondPos();

			if (isDiamondCollected(&(ship.position), &((*curr).position)) && start){
				newlyCollected += 1;
				score += 1;
				collectedIndices.push_back(i);
			}
			else{
				(*curr).ggeom.bind();
				(*curr).texture.bind();
	
				glm::mat4 M_diamond = (*curr).combinedMatrix();
				glUniformMatrix4fv(0,1,GL_FALSE, glm::value_ptr(M_diamond));

				glDrawArrays(GL_TRIANGLES, 0, 6);
				(*curr).texture.unbind();
			}
		}

		for (int i = 0; i < int(collectedIndices.size()); i++){
			uncollected_dia.erase(std::remove(uncollected_dia.begin(), uncollected_dia.end(), uncollected_dia.at(collectedIndices.at(i))), uncollected_dia.end());
		}

		ship.updateShip(user_input, newlyCollected);
		
		//Render the ship
		ship.ggeom.bind();
		ship.texture.bind();

		// Transformation
		glm::mat4 M_ship = ship.combinedMatrix();
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(M_ship));
		
		// Draw ship then unbind texture
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		ship.texture.unbind();
		
		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(5, 5));

		// Setting flags
		ImGuiWindowFlags textWindowFlags =
			ImGuiWindowFlags_NoMove |				// text "window" should not move
			ImGuiWindowFlags_NoResize |				// should not resize
			ImGuiWindowFlags_NoCollapse |			// should not collapse
			ImGuiWindowFlags_NoSavedSettings |		// don't want saved settings mucking things up
			ImGuiWindowFlags_AlwaysAutoResize |		// window should auto-resize to fit the text
			ImGuiWindowFlags_NoBackground |			// window should be transparent; only the text should be visible
			ImGuiWindowFlags_NoDecoration |			// no decoration; only the text should be visible
			ImGuiWindowFlags_NoTitleBar;			// no title; only the text should be visible

		// Begin a new window with these flags. (bool *)0 is the "default" value for its argument.
		ImGui::Begin("scoreText", (bool *)0, textWindowFlags);

		
		if (score == maxScore){
			ImGui::SetWindowFontScale(3.0f);
			ImGui::Text("You win!");
		}
		else{
			ImGui::SetWindowFontScale(1.5f);
			ImGui::Text("Score: %d", score); // Second parameter gets passed into "%d"
		}
		
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.swapBuffers();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
