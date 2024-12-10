#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>
#include <fstream>

Texture::Texture(std::string path, GLint interpolation)
	: textureID(), path(path), interpolation(interpolation)
{
	int numComponents;
	stbi_set_flip_vertically_on_load(true);
	const char* pathData = path.c_str();

	// Check if the file exists
	std::ifstream file(path);
	if (!file.good()) {
		throw std::runtime_error("Texture file not found: " + path);
	}

	unsigned char* data = stbi_load(pathData, &width, &height, &numComponents, 0);
	if (data != nullptr) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // Set alignment to 1

		bind();

		GLuint format = GL_RGB;
		switch (numComponents) {
		case 4:
			format = GL_RGBA;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 2:
			format = GL_RG;
			break;
		case 1:
			format = GL_RED;
			break;
		default:
			std::cerr << "Invalid Texture Format" << std::endl;
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

		unbind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  // Reset alignment
		stbi_image_free(data);
	}
	else {
		const char* reason = stbi_failure_reason();
		std::cerr << "STB image load failed: " << reason << std::endl;
		throw std::runtime_error("Failed to read texture data from file: " + path);
	}
}
