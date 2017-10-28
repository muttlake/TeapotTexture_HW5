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

uniform sampler2D brickTexture;

out vec4 fColor;

void main() {

	//Diffuse Color
	float Kd = 1.0f;

	vec3 diff1 = lc1 * max(dot(n, ld1), 0.0);
	vec3 diff2 = lc2 * max(dot(n, ld2), 0.0);
	vec3 diff3 = lc3 * max(dot(n, ld3), 0.0);

	vec3 diffuse = Kd * (diff1 + diff2 + diff3);

	vec4 diffuseVec4 = vec4(diffuse, 0.0f);

	//Texture color
	vec4 brickColor = texture2D(brickTexture, t);
	vec4 lightedBrickColor = diffuseVec4 * brickColor;

	//set output color for fragment for display in openGL
	//fColor = vec4(t, 1.0f, 1.0f) * vec4(1.0f, 1.0f, 1.0f, 1.0f) ;
	//fColor = brickColor;
	fColor = lightedBrickColor;
}