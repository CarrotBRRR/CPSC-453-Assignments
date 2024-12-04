#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 tex_coords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 fragPos;
out vec3 fragColor;
out vec3 n;

out vec2 tex_c;

void main() {
	fragPos = pos;
	fragColor = color;
	n = normal;

	tex_c = tex_coords;

	gl_Position = P * V * M * vec4(pos, 1.0);
}
