#include "Planet.h"
#include "glm/gtc/type_ptr.hpp"

#include "Log.h"

Planet::Planet(const std::string& texture_path, float scale, float orbital_radius, float orbital_speed, float spin_speed, float orbital_angle, float axis_angle, Planet* parent)
		: texture(texture_path, GL_LINEAR),
        scale(scale), 
        orbital_radius(orbital_radius),
        orbital_speed(orbital_speed),
        spin_speed(spin_speed),
        orbital_angle(orbital_angle),
        axis_angle(axis_angle),
        parent(parent)
{
	model_matrix = glm::mat4(1.f);
}

void Planet::update(float dt){
	model_matrix = glm::mat4(1.f);

    if(parent != nullptr){
        model_matrix = parent->model_matrix;
    }

	// Inclination
	model_matrix = glm::rotate(model_matrix, orbital_angle, glm::vec3(1.f, 0.f, 0.f));

    // Orbital Speed about the parent
	float orbit_dtheta = orbital_speed * dt;

    // Orbital Radius
	float orbit_x = orbital_radius * cos(orbit_dtheta);
	float orbit_z = orbital_radius * sin(orbit_dtheta);
	model_matrix = glm::translate(model_matrix, glm::vec3(orbit_x, 0.f, orbit_z));

	position = glm::vec3(model_matrix[3]);

	// Axial Tilt
	model_matrix = glm::rotate(model_matrix, axis_angle, glm::vec3(0.f, 0.f, 1.f));

	// Self Rotation
	float spin_dtheta = spin_speed * dt;
	model_matrix = glm::rotate(model_matrix, spin_dtheta, glm::vec3(0.f, 1.f, 0.f));

    // Scale
	model_matrix = glm::scale(model_matrix, glm::vec3(scale));
}

void Planet::draw(UnitSphere& sphere, GLint uniMat, GLint tex_coords){
    texture.bind();
    glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glDrawArrays(GL_TRIANGLES, 0, sphere.m_size);
    texture.unbind();
}
