#version 330 core

in vec3 fragPos;
in vec3 fragColor;
in vec3 n;
in vec2 tex_c;

uniform sampler2D sampler;

uniform vec3 light_pos;
uniform vec3 view_pos;

uniform vec3 ambient_colour;
uniform vec3 diffuse_colour;
uniform vec3 specular_colour;

uniform float ambient_strength;
uniform int addDiffuse;

out vec4 color;

void main() {
	vec4 tex_col = texture(sampler, tex_c);

	if (tex_col.a < 0.01) {
		discard;
	}

	vec3 norm = normalize(n);
	vec3 light_dir = normalize(light_pos - fragPos);

	// Specular
	float _specular = 0.0;
	if(dot(norm, light_dir) > 0.0) {
		vec3 view_dir = normalize(view_pos - fragPos);
		vec3 reflect_dir = reflect(-light_dir, norm);
		_specular = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	}
	vec3 specular = _specular * specular_colour * 0.5;

	// Diffuse
	vec3 diffuse = vec3(0.0);
	if (addDiffuse != 0) {
		float _diffuse = max(dot(norm, light_dir), 0.0);
		diffuse = _diffuse * diffuse_colour;
	}

	// Ambient
	vec3 ambient = ambient_strength * ambient_colour;

	vec3 res = (ambient + diffuse + specular) * tex_col.rgb;
	color = vec4(res, tex_col.a);
}
