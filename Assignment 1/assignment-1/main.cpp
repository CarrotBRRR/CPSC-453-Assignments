#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"

#include "Sierpinski.h"
#include "Pythagoras.h"
#include "Koch.h"
#include "Dragon.h"
#include "Hilbert.h"

int level = 1;
int display_mode = 0;

void level_up() {
	if (level < 8) {
		level++;
	}
}

void level_down() {
	if (level > 0) {
		level--;
	}
}

class MyCallbacks : public CallbackInterface {

public:
	MyCallbacks(ShaderProgram& shader) : shader(shader) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			shader.recompile();
		}

		else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			level_up();
			std::cout << "Level: " << level << std::endl;
		}
		else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			level_down();
			std::cout << "Level: " << level << std::endl;
		}
		else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			display_mode = (((display_mode - 1) % 4) + 4) % 4;
			std::cout << "Display Mode: " << display_mode << std::endl;
		}
		else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			display_mode = (display_mode + 1) % 4;
			std::cout << "Display Mode: " << display_mode << std::endl;
		}
	}

private:
	ShaderProgram& shader;	
};

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	Window window(800, 800, "CPSC 453"); // can set callbacks at construction if desired

	GLDebug::enable();

	// SHADERS
	ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

	// Sierpinski triangle
	Sierpinski sierpinski(level);

	// Pythagoras tree
	Pythagoras pythagoras(level);

	// Koch snowflake
	Koch koch(level);

	// Dragon curve
	Dragon dragon(level);

	// Hilbert curve
	Hilbert hilbert(level);

	// Draw Array Style
	int draw_style = 0;

	// CALLBACKS
	window.setCallbacks(std::make_shared<MyCallbacks>(shader)); // can also update callbacks to new ones

	// GEOMETRY
	CPU_Geometry cpuGeom;
	GPU_Geometry gpuGeom;

	// RENDER LOOP
	while (!window.shouldClose()) {
		glfwPollEvents();

		// Generate the Sierpinski triangle
		switch (display_mode) {
		case 0:
			sierpinski.setDepth(level);
			sierpinski.generate(cpuGeom.verts, cpuGeom.cols);
			draw_style = GL_TRIANGLES;
			break;

		case 1:
			pythagoras.setDepth(level);
			pythagoras.generate(cpuGeom.verts, cpuGeom.cols);
			draw_style = GL_TRIANGLES;
			break;

		case 2:
			koch.setDepth(level);
			koch.generate(cpuGeom.verts, cpuGeom.cols);
			draw_style = GL_LINES;
			break;

		case 3:
			dragon.setDepth(level);
			dragon.generate(cpuGeom.verts, cpuGeom.cols);
			draw_style = GL_LINES;
			break;

		case 4:
			hilbert.setDepth(level);
			hilbert.generate(cpuGeom.verts, cpuGeom.cols);
			draw_style = GL_LINES;
			break;

		default:
			cpuGeom.verts.clear();
			cpuGeom.cols.clear();
			break;
		}
		
		// Send the updated vertices and colors to the GPU
		gpuGeom.setVerts(cpuGeom.verts);
		gpuGeom.setCols(cpuGeom.cols);

		shader.use();
		gpuGeom.bind();

		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(draw_style, 0, cpuGeom.verts.size());

		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
		window.swapBuffers();
	}

	glfwTerminate();
	return 0;
}
