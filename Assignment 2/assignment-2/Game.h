#pragma once

#include "GLHandles.h"
#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"

// Game logic
bool isDiamondCollected(glm::vec3* shipPos, glm::vec3* diamondPos){
	float distance = glm::length(*shipPos - *diamondPos);

	if (distance <= 0.075){
		return true;
	}
	return false;
}

void resetGame(GameObject* ship, std::vector<GameObject*>* uncollected_dia_base, std::vector<GameObject*>* uncollected_dia){
	// Reset ship
	(*ship).position = glm::vec3(0.f,0.f,0.f);
	(*ship).facing = glm::vec3(0.f,1.f,0.f);

	(*ship).S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*ship).default_ship_size, (*ship).default_ship_size, 0.f));
	(*ship).R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*ship).T_Matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,0.f));

	// Reset Diamonds
	for (int i = 0; i < int((*uncollected_dia_base).size()); i++){
		GameObject* new_dia = (*uncollected_dia_base)[i];

		bool add = true;
		for (int j = 0; j < int((*uncollected_dia).size()); j++){
			if ((*uncollected_dia)[j] == new_dia){
				add = false;
				break;
			}
		}

		if (add){
			uncollected_dia->push_back(new_dia);
		}
	}

	// Reseting the individual diamond transforms
	(*uncollected_dia)[0]->position = glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f);
	(*uncollected_dia)[0]->facing = glm::vec3(0.5f,0.25f,0.f);
	(*uncollected_dia)[0]->S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*uncollected_dia)[0]->default_diamond_size, (*uncollected_dia)[0]->default_diamond_size, 0.f));
	(*uncollected_dia)[0]->R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*uncollected_dia)[0]->T_Matrix = glm::translate(glm::mat4(1.f), (*uncollected_dia)[0]->position);

	(*uncollected_dia)[1]->position = glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f);
	(*uncollected_dia)[1]->facing = glm::vec3(0.25f,0.5f,0.f);
	(*uncollected_dia)[1]->S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*uncollected_dia)[0]->default_diamond_size, (*uncollected_dia)[0]->default_diamond_size, 0.f));
	(*uncollected_dia)[1]->R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*uncollected_dia)[1]->T_Matrix = glm::translate(glm::mat4(1.f), (*uncollected_dia)[1]->position);

	(*uncollected_dia)[2]->position = glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f);
	(*uncollected_dia)[2]->facing = glm::vec3(-0.5f,-0.5f,0.f);
	(*uncollected_dia)[2]->S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*uncollected_dia)[0]->default_diamond_size, (*uncollected_dia)[0]->default_diamond_size, 0.f));
	(*uncollected_dia)[2]->R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*uncollected_dia)[2]->T_Matrix = glm::translate(glm::mat4(1.f), (*uncollected_dia)[2]->position);

	(*uncollected_dia)[3]->position = glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f);
	(*uncollected_dia)[3]->facing = glm::vec3(-0.15f,0.35f,0.f);
	(*uncollected_dia)[3]->S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*uncollected_dia)[0]->default_diamond_size, (*uncollected_dia)[0]->default_diamond_size, 0.f));
	(*uncollected_dia)[3]->R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*uncollected_dia)[3]->T_Matrix = glm::translate(glm::mat4(1.f), (*uncollected_dia)[3]->position);

	(*uncollected_dia)[4]->position = glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f);
	(*uncollected_dia)[4]->facing = glm::vec3(0.5f,-0.5f,0.f);
	(*uncollected_dia)[4]->S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*uncollected_dia)[0]->default_diamond_size, (*uncollected_dia)[0]->default_diamond_size, 0.f));
	(*uncollected_dia)[4]->R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*uncollected_dia)[4]->T_Matrix = glm::translate(glm::mat4(1.f), (*uncollected_dia)[4]->position);

	(*uncollected_dia)[4]->position = glm::vec3(((float) rand() / (RAND_MAX)) - 0.5f, ((float) rand() / (RAND_MAX)) - 0.5f, 0.f);
	(*uncollected_dia)[4]->facing = glm::vec3(0.5f,-0.5f,0.f);
	(*uncollected_dia)[4]->S_Matrix = glm::scale(glm::mat4(1.f), glm::vec3((*uncollected_dia)[0]->default_diamond_size, (*uncollected_dia)[0]->default_diamond_size, 0.f));
	(*uncollected_dia)[4]->R_matrix = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f,0.f,1.f));
	(*uncollected_dia)[4]->T_Matrix = glm::translate(glm::mat4(1.f), (*uncollected_dia)[4]->position);
}
