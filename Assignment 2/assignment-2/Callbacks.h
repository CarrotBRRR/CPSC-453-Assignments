#pragma once

#include "GLHandles.h"
#include <glad/glad.h>
#include <string>

// CALLBACKS
class MyCallbacks : public CallbackInterface {
public:
	MyCallbacks(ShaderProgram& shader) : shader(shader) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		float move_speed = 0.015f;

		if (action == GLFW_PRESS || action == GLFW_REPEAT){
			if (key == GLFW_KEY_W){
				player_inputs.move_distance += move_speed * glm::vec3(0.f, 1.f, 0.f);
			}

			if (key == GLFW_KEY_S){
				player_inputs.move_distance -= move_speed * glm::vec3(0.f, 1.f, 0.f);
			}
		}

		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			player_inputs.resetFlag = true;
			player_inputs.cursorPos = glm::vec3(0.f,1.f,0.f);
		}
	}

	// From Tutorial
	virtual void cursorPosCallback(double xpos, double ypos) {

		glm::vec4 cursor = {xpos, ypos, 0.f, 1.f};

		// std::cout << cursor.x << " " << cursor.y << std::endl;
		
		// translates by half a pixel for pixel center being considered for cursor position
		glm::mat4 pixel_centering_T = glm::translate(glm::mat4(1.f), glm::vec3(0.5f,0.5f,0.f));
		cursor = pixel_centering_T * cursor;

		// std::cout << cursor.x << " " << cursor.y << std::endl;
		
		// Scale to 0-1
		glm::mat4 zero_to_one_S = glm::scale(glm::mat4(1.f), glm::vec3(0.00125f, 0.00125f, 0.f));
		cursor = zero_to_one_S * cursor;

		// std::cout << cursor.x << " " << cursor.y << std::endl;

		// Turn y from 0-1 to -1 to 0 (since xpos and ypos record position as positive downward)
		cursor.y = 1.f - cursor.y;

		// std::cout << cursor.x << " " << cursor.y << std::endl;

		// Scale then Translate values to make them between -1 to 1
		glm::mat4 normalize_S = glm::scale(glm::mat4(1.f), glm::vec3(2.f,2.f,0.f));
		glm::mat4 normalize_T = glm::translate(glm::mat4(1.f), glm::vec3(-1.f,-1.f,0.f));
		cursor = normalize_T * normalize_S * cursor;

		// std::cout << cursor.x << " " << cursor.y << std::endl;

		player_inputs.cursorPos.x = cursor.x;
		player_inputs.cursorPos.y = cursor.y;
	}

	Params getParams(){
		Params ret = player_inputs;

		player_inputs.move_distance = {0.f,0.f,0.f};
		player_inputs.resetFlag = false;

		return ret;
	}

private:
	ShaderProgram& shader;
	Params player_inputs;
};