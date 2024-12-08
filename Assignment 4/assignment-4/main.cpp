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
#include "Planet.h"

struct Parameters {
	bool pause = false;
	bool restart = false;

	float dt = 0.f;
	float last_time = 0.f;
	float current_time = 0.f;

	float animation_speed = 1.0f;

	glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 0.0f); // Sun position

	glm::vec3 ambient_colour = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuse_colour = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);

	int focus_planet = 0;
	glm::vec3 focused_planet_loc = glm::vec3(0.0f, 0.0f, 0.0f);
};

static Parameters params;

// EXAMPLE CALLBACKS
class Assignment4 : public CallbackInterface {

public:
	Assignment4()
		: camera(glm::radians(45.f), glm::radians(45.f), 50.0)
		, aspect(1.0f)
		, rightMouseDown(false)
		, mouseOldX(0.0)
		, mouseOldY(0.0)
	{}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_P) {
				params.pause = !params.pause;
			}
			if (key == GLFW_KEY_R) {
				params.restart = true;
			}

			if (key == GLFW_KEY_1) {
				params.focus_planet = 0;
			}
			if (key == GLFW_KEY_2) {
				params.focus_planet = 1;
			}
			if (key == GLFW_KEY_3) {
				params.focus_planet = 2;
			}

			if (key == GLFW_KEY_UP) {
				params.focus_planet = (((params.focus_planet - 1) % 3) + 3) % 3;
			}
			if (key == GLFW_KEY_DOWN) {
				params.focus_planet = (params.focus_planet + 1) % 3;
			}

			if (key == GLFW_KEY_EQUAL) {
				params.animation_speed *= 1.1f;
			}
			if (key == GLFW_KEY_MINUS) {
				params.animation_speed /= 1.1f;
			}
		}
	}
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
		glm::mat4 M = glm::mat4(1.f);
		glm::mat4 V = camera.getView(params.focused_planet_loc);
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

	UnitSphere base_sphere;
	base_sphere.generateGeometry(1.f);

	// Planet Constructor(texture_path, scale, orbital_radius, orbital_speed, spin_speed, orbital_angle, axis_angle, parent)
	Planet sun("textures/8k_sun.jpg", 5.f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr);
	Planet earth("textures/8k_earth_daymap.jpg", .5f, 4.0f, 2.f, 2.f, .2f, 0.42f, &sun);
	Planet moon("textures/8k_moon.jpg", 0.25f, 4.0f, 1.0f, 10.0f, 1.0f, 0.0f, &earth);

	UnitSphere stars;
	stars.generateGeometry(100.f);
	Texture stars_tex("textures/8k_stars_milky_way.jpg", GL_LINEAR);

	GLint uniMat = glGetUniformLocation(shader, "M");

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

		GLint ambient_col_loc = glGetUniformLocation(shader, "ambient_colour");
		glUniform3fv(ambient_col_loc, 1, glm::value_ptr(params.ambient_colour));
		GLint ambient_strength_loc = glGetUniformLocation(shader, "ambient_strength");

		GLint diffuse_col_loc = glGetUniformLocation(shader, "diffuse_colour");
		glUniform3fv(diffuse_col_loc, 1, glm::value_ptr(params.diffuse_colour));
		GLint addDiffuse = glGetUniformLocation(shader, "addDiffuse");

		GLint specular_col_loc = glGetUniformLocation(shader, "specular_colour");
		glUniform3fv(specular_col_loc, 1, glm::value_ptr(params.specular_colour));

		float last_time = params.current_time;
		if (!params.pause) {
			params.current_time = glfwGetTime();
			params.dt += (params.current_time - last_time) * params.animation_speed;
			last_time = params.current_time;
		}
		else {
			glfwSetTime(params.dt);
		}

		if (params.restart) {

			// Reset Time
			glfwSetTime(0.f);
			params.dt = 0.f;
			params.last_time = 0.f;
			params.current_time = 0.f;
			params.animation_speed = 1.0f;

			// Reset Camera
			params.focus_planet = 0;
			params.focused_planet_loc = glm::vec3(0.0f, 0.0f, 0.0f);

			// Reset States
			params.pause = false;
			params.restart = false;
		}

		base_sphere.m_gpu_geom.bind();

		switch (params.focus_planet) {
		case 0:
			params.focused_planet_loc = sun.position;
			break;
		case 1:
			params.focused_planet_loc = earth.position;
			break;
		case 2:
			params.focused_planet_loc = moon.position;
			break;
		}

		// Sun
		sun.update(-params.dt);
		glUniform1f(ambient_strength_loc, 1.0f);
		glUniform1i(addDiffuse, 0);
		sun.draw(base_sphere, uniMat, 0);

		// Earth
		earth.update(-params.dt);
		glUniform1f(ambient_strength_loc, 0.025f);
		glUniform1i(addDiffuse, 1);
		earth.draw(base_sphere, uniMat, 0);

		// Moon
		moon.update(-params.dt);
		glUniform1f(ambient_strength_loc, 0.025f);
		glUniform1i(addDiffuse, 1);
		moon.draw(base_sphere, uniMat, 0);

		// Stars		
		stars.m_gpu_geom.bind();
		stars_tex.bind();
		glUniform1f(ambient_strength_loc, 1.f);
		glUniform1i(addDiffuse, 0);
		glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(100000.f)));
		glDrawArrays(GL_TRIANGLES, 0, stars.m_size);
		stars_tex.unbind();

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		window.swapBuffers();
	}
	glfwTerminate();
	return 0;
}
