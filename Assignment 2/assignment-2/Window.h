#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

class InputManager;

class Window {
public:
	Window(int width, int height);

	bool init();

	void setInputManager(InputManager* input_manager);

	bool shouldClose();

	GLFWwindow* getWindow();

private:
	int width;
	int height;

	GLFWwindow* window = nullptr;
	InputManager* input_manager = nullptr;

	static void updateWindowSize(GLFWwindow* _window, int _width, int _height);

	static void handleKeyboardInput(GLFWwindow* _window, int key, int scancode, int action, int mods);
};
