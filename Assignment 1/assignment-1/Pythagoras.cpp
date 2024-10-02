#define _USE_MATH_DEFINES

#include "Pythagoras.h"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>

Pythagoras::Pythagoras(int depth) : depth(depth) {}

void Pythagoras::setDepth(int depth) {
	this->depth = depth;
}

int Pythagoras::getDepth() const {
	return depth;
}

void Pythagoras::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours) {
	vertices.clear();
	colours.clear();

	glm::vec2 p(-0.125f, -0.75f);
	
	// Define the base colour for the initial square
	colours.push_back(glm::vec3(0.6f, 0.3f, 0.0f));
	colours.push_back(glm::vec3(0.6f, 0.3f, 0.0f));
	colours.push_back(glm::vec3(0.6f, 0.3f, 0.0f));
	colours.push_back(glm::vec3(0.6f, 0.3f, 0.0f));
	colours.push_back(glm::vec3(0.6f, 0.3f, 0.0f));
	colours.push_back(glm::vec3(0.6f, 0.3f, 0.0f));

	// Call the draw function to recursively generate the Pythagoras Tree
	draw(vertices, colours, p, 0.25f, 0.0, this->depth);
}

void Pythagoras::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colours, glm::vec2 position, float size, float angle, int depth) {
	if (depth == 0) {
		glm::vec2 p0 = position; // bottom left
		glm::vec2 p1 = p0 + glm::vec2(size * std::cos(angle), size * std::sin(angle)); // bottom right
		glm::vec2 p2 = p1 + glm::vec2(-size * std::sin(angle), size * std::cos(angle)); // top right
		glm::vec2 p3 = p0 + glm::vec2(-size * std::sin(angle), size * std::cos(angle)); // top left

		vertices.push_back(glm::vec3(p0, 0.0f));
		vertices.push_back(glm::vec3(p1, 0.0f));
		vertices.push_back(glm::vec3(p2, 0.0f));

		vertices.push_back(glm::vec3(p0, 0.0f));
		vertices.push_back(glm::vec3(p2, 0.0f));
		vertices.push_back(glm::vec3(p3, 0.0f));

		for (int i = 0; i < 6; i++) {
			colours.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
		}
	}
	else {
		glm::vec2 p0 = position;
		glm::vec2 p1 = p0 + glm::vec2(size * std::cos(angle), size * std::sin(angle));
		glm::vec2 p2 = p1 + glm::vec2(-size * std::sin(angle), size * std::cos(angle));
		glm::vec2 p3 = p0 + glm::vec2(-size * std::sin(angle), size * std::cos(angle));

		vertices.push_back(glm::vec3(p0, 0.0f));
		vertices.push_back(glm::vec3(p1, 0.0f));
		vertices.push_back(glm::vec3(p2, 0.0f));

		vertices.push_back(glm::vec3(p0, 0.0f));
		vertices.push_back(glm::vec3(p2, 0.0f));
		vertices.push_back(glm::vec3(p3, 0.0f));

		for (int i = 0; i < 6; i++) {
			colours.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
		}

		float newSize = size / sqrt(2.0f); // size of next square

		float left_angle = angle + M_PI_4;
		glm::vec2 left_position = p3;
		draw(vertices, colours, left_position, newSize, left_angle, depth - 1);

		float right_angle = angle - M_PI_4;
		glm::vec2 right_position = p2 - glm::vec2(newSize * std::cos(right_angle), newSize * std::sin(right_angle));
		draw(vertices, colours, right_position, newSize, right_angle, depth - 1);
	}
}

