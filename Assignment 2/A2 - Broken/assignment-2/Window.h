#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class Input;

class Window {
public:
	Window(int width, int height);

	bool init();

	void setInputManager(Input* input_manager);

	bool shouldClose();

	GLFWwindow* getWindow();

private:
	int width;
	int height;

	GLFWwindow* window = nullptr;
	Input* input_manager = nullptr;

	static void updateWindowSize(GLFWwindow* window, int _width, int _height);

	static void handleKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods);
};
