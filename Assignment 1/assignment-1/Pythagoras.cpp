
#include "Pythagoras.h"
#include <iostream>
#include <glm/glm.hpp>

Pythagoras::Pythagoras(int depth) : depth(depth) {}

void Pythagoras::setDepth(int depth) {
	this->depth = depth;
}

int Pythagoras::getDepth() const {
	return depth;
}

void Pythagoras::generate(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors) {
	vertices.clear();
	colors.clear();

	// Define the initial square with four corners
	glm::vec3 p1(-0.5f, -0.5f, 1.f);
	glm::vec3 p2(0.5f, -0.5f, 1.f);
	glm::vec3 p3(0.5f, 0.5f, 1.f);
	glm::vec3 p4(-0.5f, 0.5f, 1.f);

	// Define the base color for the initial square
	glm::vec3 base_color(0.5f, 0.5f, 0.5f);

	// Call the draw function to recursively generate the Pythagoras Tree
	draw(vertices, colors, this->depth, p1, p2, p3, p4, base_color);
}

void Pythagoras::draw(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, int depth, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 color) {
	if (depth == 0) {
		// Define two triangles for the square using GL_TRIANGLE_STRIP
		vertices.push_back(p1);
		vertices.push_back(p2);
		vertices.push_back(p4);

		vertices.push_back(p4);
		vertices.push_back(p2);
		vertices.push_back(p3);

		// Assign the same color for all vertices
		for (int i = 0; i < 6; i++) {
			colors.push_back(color);
		}
	}
	else {
		// Calculate side length of the current square (hypotenuse for new squares)
		glm::vec3 side = p2 - p1; // Vector from p1 to p2
		float h = glm::length(side);  // Length of the base square side (hypotenuse)
		float s = h / glm::sqrt(2.0f); // Calculate the new side length (s = h / √2)

		// Compute direction vectors for rotation
		glm::vec3 dir = glm::normalize(side);
		glm::vec3 rot_left(-dir.y, dir.x, 0.0f); // Perpendicular vector for the left square
		glm::vec3 rot_right(dir.y, -dir.x, 0.0f); // Perpendicular vector for the right square

		// Calculate new points for the two smaller squares
		glm::vec3 new_p1_left = p4 + rot_left * s;  // New top-left corner
		glm::vec3 new_p2_left = p4;                  // Keep the top corner the same
		glm::vec3 new_p3_left = new_p1_left + dir * s; // New bottom-left corner
		glm::vec3 new_p4_left = new_p1_left + rot_left * s; // New bottom-right corner

		glm::vec3 new_p1_right = p3;                 // Keep the top corner the same
		glm::vec3 new_p2_right = p3 + rot_right * s; // New top-right corner
		glm::vec3 new_p3_right = new_p2_right + dir * s; // New bottom-right corner
		glm::vec3 new_p4_right = p2 + rot_right * s; // New bottom-left corner

		glm::vec3 new_color(0.8f, 0.3f, 0.3f);  // Example color for recursive layers

		// Recursively draw the two smaller squares for left and right sides
		draw(vertices, colors, depth - 1, p4, new_p1_left, new_p4_left, new_p3_left, new_color); // Left square
		draw(vertices, colors, depth - 1, new_p1_right, p3, new_p2_right, new_p2_right, new_color); // Right square
	}
}

