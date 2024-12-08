#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

#include "UnitSphere.h"
#include "Texture.h"


class Planet {
    public:
        glm::mat4 model_matrix;
        Texture texture;

        float scale;

        float orbital_radius;
        float orbital_speed;
        float orbital_angle;

        float spin_speed;
        float axis_angle;

		glm::vec3 position;

        Planet* parent;

        Planet(
            const std::string& texture_path, 
            float scale, 
            float orbital_radius, 
            float orbital_speed, 
            float spin_speed, 
            float orbital_angle, 
            float axis_angle, 
            Planet* parent
        );

        void update(float dt);

        void draw(UnitSphere& sphere, GLint uniMat, GLint tex_coords);

};
