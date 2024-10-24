#include "Window.h"
#include "Input.h"

#include <iostream>

Window::Window(int width, int height) {
	this->width = width;
	this->height = height;
}

bool Window::init() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glfwSetFramebufferSizeCallback(window, updateWindowSize);
	return true;
}

void Window::setInputManager(Input* input_manager) {
	this->input_manager = input_manager;
	glfwSetKeyCallback(window, handleKeyboardInput);
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

GLFWwindow* Window::getWindow() {
	return window;
}

void Window::updateWindowSize(GLFWwindow* window, int _width, int _height) {
	Input* _input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (_input != nullptr)
	{
		_input->windowSizeInput(_width, _height);
	}

	glViewport(0, 0, _width, _height);
}

void Window::handleKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Input* _input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (_input != nullptr)
	{
		_input->keyboardInput(key, scancode, action, mods);
	}
}

