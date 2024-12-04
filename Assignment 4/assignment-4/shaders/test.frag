#version 330 core

in vec3 fragPos;
in vec3 fragColor;
in vec3 n;

//uniform vec3 lightPosition;

in vec2 tex_c;
uniform sampler2D sampler;

out vec4 color;

void main() {
	vec4 d = texture(sampler, tex_c);

	if (d.a < 0.01) {
		discard;
	}

	// color = vec4(fragColor, 1.0);
	color = d;
}
