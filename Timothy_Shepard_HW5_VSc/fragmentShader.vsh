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

//uniform sampler2D brickTexture;
//uniform sampler2D bumpTexture;
uniform sampler2D brickTexture;
uniform sampler2D bumpTexture;

out vec4 fColor;

void main() {

	// Do Bump Mapping
	vec3 bumpNormal = texture2D(bumpTexture, t).rgb * 2.0 - 1.0;
	vec3 bumpNormal2 = normalize(bumpNormal);
	//vec3 turbNormal = vec3(n.x + bumpNormal2.x, n.y + bumpNormal2.y, n.z + bumpNormal2.z);
	//turbNormal = normalize(turbNormal);
	vec3 turbNormal = n + bumpNormal;
	vec3 turbNormal2 = normalize(turbNormal);

	//Diffuse Color
	float Kd = 1.0f;

	//n = normalize(n);
	//vec3 diff1 = lc1 * max(dot(n, ld1), 0.0);
	//vec3 diff2 = lc2 * max(dot(n, ld2), 0.0);
	//vec3 diff3 = lc3 * max(dot(n, ld3), 0.0);

	//vec3 diff1 = lc1 * max(dot(bumpNormal, ld1), 0.0);  // this is not the right one because
	//vec3 diff2 = lc2 * max(dot(bumpNormal, ld2), 0.0);  // it means lighting doesnt matter
	//vec3 diff3 = lc3 * max(dot(bumpNormal, ld3), 0.0);

	vec3 diff1 = lc1 * max(dot(turbNormal2, ld1), 0.0);
	vec3 diff2 = lc2 * max(dot(turbNormal2, ld2), 0.0);
	vec3 diff3 = lc3 * max(dot(turbNormal2, ld3), 0.0);

	vec3 diffuse = Kd * (diff1 + diff2 + diff3);

	////Ambient Color
	//float Ka = 0.1f;

	//vec3 amb1 = lc1;
	//vec3 amb2 = lc2;
	//vec3 amb3 = lc3;

	//vec3 ambient = Ka * (amb1 + amb2 + amb3);
	////vec3 ambient = Ka * amb3;

	////calculate output color
	//vec4 c = vec4(( diffuse + ambient), 1.0f);

	vec4 diffuseVec4 = vec4(diffuse, 0.0f);

	//Texture color
	vec4 brickColor = texture2D(brickTexture, t);
	vec4 bumpColor = texture2D(bumpTexture, t);
	vec4 lightedBrickColor = diffuseVec4 * brickColor;
	vec4 lightedBumpColor = diffuseVec4 * bumpColor;



	//set output color for fragment for display in openGL
	//fColor = lightedBumpColor;
	fColor = lightedBrickColor;
}