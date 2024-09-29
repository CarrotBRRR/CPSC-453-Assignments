#include "Dragon.h"
#include <iostream>
#include <glm/glm.hpp>

Dragon::Dragon(int depth) : depth(depth) {}

void Dragon::setDepth(int depth) {
	this->depth = depth;
}

int Dragon::getDepth() const {
	return depth;
}

void Dragon::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors) {
	vertices.clear();
	colors.clear();

	// Starting points for the Dragon Curve
	glm::vec3 p1(-0.5f, 0.0f, 0.0f);
	glm::vec3 p2(0.5f, 0.0f, 0.0f);

	// Base color for the Dragon Curve
	glm::vec3 base_color(1.0f, 0.5f, 0.0f); // Orange color

	// Generate the Dragon Curve
	draw(vertices, colors, depth, p1, p2, base_color);
}

void Dragon::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 color) {
	if (depth == 0) {
		// Base case: add the line segment to the vertices
		vertices.push_back(p1);
		vertices.push_back(p2);

		// Assign color to the vertices
		colors.push_back(color);
		colors.push_back(color);
	}
	else {
		// Calculate the midpoint of the segment
		glm::vec3 segment = p2 - p1;
		glm::vec3 mid = (p1 + p2) / 2.0f;

		// Create a triangle with the original segment as the hypotenuse
		glm::vec3 normal = glm::cross(segment, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 p3 = mid + glm::normalize(normal) * glm::length(segment) / 2.0f;

		// Recursively draw the Dragon Curve
		draw(vertices, colors, depth - 1, p1, p3, color);
		draw(vertices, colors, depth - 1, p2, p3, color);
	}
}
