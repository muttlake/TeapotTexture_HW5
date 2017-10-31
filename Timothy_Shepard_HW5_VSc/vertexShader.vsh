#version 450

in vec3 s_vPosition;
in vec2 s_vTexCoord;
in vec3 s_vNormal;

uniform mat4 MVP;   // MVP Matrix
uniform mat4 MV;    // Model View Matrix

uniform vec3 light1direction;
uniform vec3 light2direction;
uniform vec3 light3direction;

uniform vec3 light1color;
uniform vec3 light2color;
uniform vec3 light3color;


//uniform sampler2D bumpTexture;


out vec3 v;
out vec2 t;
out vec3 n;

out vec3 ld1;
out vec3 ld2;
out vec3 ld3;

out vec3 lc1;
out vec3 lc2;
out vec3 lc3;

void main() {

	//Doing Phong Shading like HW4 but also adding texture
	v = (MV * vec4(s_vPosition, 1.0f)).xyz;
	t = s_vTexCoord;

	n = (transpose(inverse(MV))* vec4(s_vNormal, 0.0f)).xyz;
	n = normalize(n);

	//// Do Bump Mapping
	//vec3 bumpNormal = texture2D(bumpTexture, t).rgb * 2.0 - 1.0;
	//vec3 bumpNormal2 = normalize(bumpNormal);
	//vec3 turbNormal = vec3(n.x + bumpNormal2.x, n.y + bumpNormal2.y, n.z + bumpNormal2.z);
	////turbNormal = normalize(turbNormal);
	////vec3 turbNormal = n + bumpNormal;
	//vec3 turbNormal2 = normalize(turbNormal);
	//n = turbNormal2;

	ld1 = -1 * (MV * vec4(light1direction, 0.0f)).xyz;
	ld1 = normalize(ld1);

	ld2 = -1 * (MV * vec4(light2direction, 0.0f)).xyz;
	ld2 = normalize(ld2);

	ld3 = -1 * (MV * vec4(light3direction, 0.0f)).xyz;
	ld3 = normalize(ld3);

	lc1 = light1color;
	lc2 = light2color;
	lc3 = light3color;

	//set vertex position for display in openGL
	gl_Position = MVP * vec4(s_vPosition, 1.0f);
}