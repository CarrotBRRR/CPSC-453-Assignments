//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <functional>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "UnitSphere.h"

// EXAMPLE CALLBACKS
class Assignment4 : public CallbackInterface {

public:
	Assignment4()
		: camera(glm::radians(45.f), glm::radians(45.f), 3.0)
		, aspect(1.0f)
		, rightMouseDown(false)
		, mouseOldX(0.0)
		, mouseOldY(0.0)
	{}

	virtual void keyCallback(int key, int scancode, int action, int mods) {}
	virtual void mouseButtonCallback(int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (action == GLFW_PRESS)			rightMouseDown = true;
			else if (action == GLFW_RELEASE)	rightMouseDown = false;
		}
	}
	virtual void cursorPosCallback(double xpos, double ypos) {
		if (rightMouseDown) {
			camera.incrementTheta(ypos - mouseOldY);
			camera.incrementPhi(xpos - mouseOldX);
		}
		mouseOldX = xpos;
		mouseOldY = ypos;
	}
	virtual void scrollCallback(double xoffset, double yoffset) {
		camera.incrementR(yoffset);
	}
	virtual void windowSizeCallback(int width, int height) {
		// The CallbackInterface::windowSizeCallback will call glViewport for us
		CallbackInterface::windowSizeCallback(width,  height);
		aspect = float(width)/float(height);
	}

	void viewPipeline(ShaderProgram &sp) {
		glm::mat4 M = glm::mat4(1.0);
		glm::mat4 V = camera.getView();
		glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.01f, 1000.f);
		//GLint location = glGetUniformLocation(sp, "lightPosition");
		//glm::vec3 light = camera.getPos();
		//glUniform3fv(location, 1, glm::value_ptr(light));
		GLint uniMat = glGetUniformLocation(sp, "M");
		glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(M));
		uniMat = glGetUniformLocation(sp, "V");
		glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(V));
		uniMat = glGetUniformLocation(sp, "P");
		glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(P));
	}
	Camera camera;
private:
	bool rightMouseDown;
	float aspect;
	double mouseOldX;
	double mouseOldY;
};

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	Window window(800, 800, "CPSC 453 - Assignment 4");

	GLDebug::enable();

	// CALLBACKS
	auto a4 = std::make_shared<Assignment4>();
	window.setCallbacks(a4);

	ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

	UnitSphere earth;
	earth.generateGeometry(1.0f);

	UnitSphere moon;
	moon.generateGeometry(0.25f);

	UnitSphere sun;
	sun.generateGeometry(5.0f);

	UnitSphere stars;
	stars.generateGeometry(500.0f);

	// TEXTURES
	Texture sun_tex("textures/8k_sun.jpg", GL_LINEAR);
	Texture moon_tex("textures/8k_moon.jpg", GL_LINEAR);
	Texture earth_tex("textures/8k_earth_daymap.jpg", GL_LINEAR);
	Texture stars_tex("textures/8k_stars_milky_way.jpg", GL_LINEAR);

	// RENDER LOOP
	while (!window.shouldClose()) {
		glfwPollEvents();

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL /*GL_LINE*/);

		shader.use();

		a4->viewPipeline(shader);

		// Model Matrices
		GLint uniMat = glGetUniformLocation(shader, "M");
		glm::mat4 earth_model = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 0.0f, 0.0f));  
		glm::mat4 moon_model = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 0.0f, 0.0f)); 

		// Sun
		sun.m_gpu_geom.bind();
		sun_tex.bind();
		glDrawArrays(GL_TRIANGLES, 0, sun.m_size);
		sun_tex.unbind();

		// Moon
		moon.m_gpu_geom.bind();
		moon_tex.bind();
		glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(moon_model));
		glDrawArrays(GL_TRIANGLES, 0, moon.m_size);
		moon_tex.unbind();

		// Earth
		earth.m_gpu_geom.bind();
		earth_tex.bind();
		glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(earth_model));
		glDrawArrays(GL_TRIANGLES, 0, earth.m_size);
		earth_tex.unbind();

		// Stars		
		stars.m_gpu_geom.bind();
		stars_tex.bind();
		glDrawArrays(GL_TRIANGLES, 0, stars.m_size);
		stars_tex.unbind();

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		window.swapBuffers();
	}
	glfwTerminate();
	return 0;
}
