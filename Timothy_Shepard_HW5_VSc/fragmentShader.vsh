#version 450

in vec3 v;
in vec2 t;
in vec3 n;

in vec3 ld1;
in vec3 ld2;
in vec3 ld3;

in vec3 lc1;
in vec3 lc2;
in vec3 lc3;

out vec4 fColor;

void main() {

	//set output color for fragment for display in openGL
	fColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}