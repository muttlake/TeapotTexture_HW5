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
uniform sampler2D bumpTexture;

out vec4 fColor;

void main() {

	// Apply bump texture to normal
	// lookup normal from normal map, move from [0,1] to  [-1, 1] range, normalize
	//vec3 normal = 2.0 * texture2D (normalTexture, gl_TexCoord[0].st).rgb - 1.0;
	//normal = normalize(normal);
	vec3 bumpNormal = 2.0 * texture2D(bumpTexture, t).rgb - 1.0;
	bumpNormal = normalize(bumpNormal);


	//Diffuse Color
	float Kd = 1.0f;

	//vec3 diff1 = lc1 * max(dot(n, ld1), 0.0);
	//vec3 diff2 = lc2 * max(dot(n, ld2), 0.0);
	//vec3 diff3 = lc3 * max(dot(n, ld3), 0.0);

	vec3 diff1 = lc1 * max(dot(bumpNormal, ld1), 0.0);
	vec3 diff2 = lc2 * max(dot(bumpNormal, ld2), 0.0);
	vec3 diff3 = lc3 * max(dot(bumpNormal, ld3), 0.0);

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