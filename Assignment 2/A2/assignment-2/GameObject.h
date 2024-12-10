#pragma once

#include "GLHandles.h"
#include <glad/glad.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Geometry.h"

struct Params{
	glm::vec3 move_distance = {0.f,0.f,0.f};
	glm::vec3 cursorPos = {0.f,1.f,0.f};
	bool resetFlag = false;
};

struct GameObject {
	// Ship Constructor
	GameObject(std::string texturePath, GLenum textureInterpolation) :
		texture(texturePath, textureInterpolation),
		position(0.0f, 0.0f, 0.0f),
		facing(0.f, 1.f, 0.f)
	{
		
		cgeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(-1.f, -1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(1.f, 1.f, 0.f));

		cgeom.texCoords.push_back(glm::vec2(0.f, 1.f));
		cgeom.texCoords.push_back(glm::vec2(0.f, 0.f));
		cgeom.texCoords.push_back(glm::vec2(1.f, 0.f));
		cgeom.texCoords.push_back(glm::vec2(0.f, 1.f));
		cgeom.texCoords.push_back(glm::vec2(1.f, 0.f));
		cgeom.texCoords.push_back(glm::vec2(1.f, 1.f));

		ggeom.setVerts(cgeom.verts);
		ggeom.setTexCoords(cgeom.texCoords);

		S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3(default_ship_size, default_ship_size, 0.f));
		T_Matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,0.f));
		R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
		
	}

	// Diamond Constructor
	GameObject(std::string texturePath, GLenum textureInterpolation, glm::vec3 pos, glm::vec3 face) :
		texture(texturePath, textureInterpolation),
		position(pos),
		facing(face)
	{
		cgeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(-1.f, -1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(-1.f, 1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(1.f, -1.f, 0.f));
		cgeom.verts.push_back(glm::vec3(1.f, 1.f, 0.f));

		cgeom.texCoords.push_back(glm::vec2(0.f, 1.f));
		cgeom.texCoords.push_back(glm::vec2(0.f, 0.f));
		cgeom.texCoords.push_back(glm::vec2(1.f, 0.f));
		cgeom.texCoords.push_back(glm::vec2(0.f, 1.f));
		cgeom.texCoords.push_back(glm::vec2(1.f, 0.f));
		cgeom.texCoords.push_back(glm::vec2(1.f, 1.f));

		ggeom.setVerts(cgeom.verts);
		ggeom.setTexCoords(cgeom.texCoords);

		S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3(default_diamond_size, default_diamond_size, 0.f));
		T_Matrix = glm::translate(glm::mat4(1.f), pos);
		R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	}

	CPU_Geometry cgeom;
	GPU_Geometry ggeom;
	Texture texture;

	float default_ship_size = 0.15f;
	float default_diamond_size = 0.1f;

	glm::vec3 position;
	glm::vec3 facing;
	glm::mat4 S_Matrix;
	glm::mat4 R_matrix;
	glm::mat4 T_Matrix;

	glm::mat4 combinedMatrix(){
		return T_Matrix * R_matrix * S_Matrix;
	}

	void updateShip(Params user_input, int counter){
		// Compute rotation changes
		float angle;
		glm::vec3 look = user_input.cursorPos - this->position;
		
		if ( (look.x * look.x + look.y * look.y)  > 0.001){
			glm::vec3 face_uvector = glm::normalize(look);
			
			// Compute the signed angle using atan2
			angle = atan2(face_uvector.y, face_uvector.x) - atan2(facing.y, facing.x);

			// face direction
			this->facing = face_uvector;

			// Update the rotation matrix
			glm::mat4 added_R = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f,0.f,1.f));
			this->R_matrix = added_R * this->R_matrix;
		}

		// Compute translation changes
		glm::vec3 move_distance = user_input.move_distance.y * this->facing; // Move in the direction you're facing 

		// Update position if not going out of bounds
		if ((!(abs((this->position + move_distance).x) >= 1.0f || abs((this->position + move_distance).y) >= 1.0f)) 
		&& ((user_input.cursorPos - (this->position + move_distance)).x * (user_input.cursorPos - (this->position + move_distance)).x 
		+ (user_input.cursorPos - (this->position + move_distance)).y * (user_input.cursorPos - (this->position + move_distance)).y > 0.001) ){
			this->position += move_distance;

			glm::mat4 added_T = glm::translate(glm::mat4(1.0f), move_distance);
			this->T_Matrix = added_T * this->T_Matrix;
		}
		

		// Change the transformation matrices
		glm::mat4 added_S = glm::scale(glm::mat4(1.f), glm::vec3(1.f + counter * 0.1, 1.f + counter * 0.1, 1.f));
		this->S_Matrix = added_S * this->S_Matrix;
		
	}

	void updateDiamondPos(){
		glm::vec3 move_distance = this->facing * 0.016f;
		// if not going out of bounds, proceed. else reverse direction
		if (!(abs((this->position + move_distance).x) >= 1.0f || abs((this->position + move_distance).y) >= 1.0f)){
			this->position += move_distance;
		}
		else {
			this->facing = -this->facing;
		}

		glm::mat4 added_T = glm::translate(glm::mat4(1.f), move_distance);
		this->T_Matrix = added_T * this->T_Matrix;
	}
};