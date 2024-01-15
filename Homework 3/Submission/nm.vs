#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vs_out.FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	vs_out.TexCoords = aUV;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNorm);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));

	// Transform lightPos and viewPos into model space first
	vec3 modelLightPos = vec3(inverse(modelMatrix) * vec4(lightPos, 1.0));
	vec3 modelViewPos = vec3(inverse(modelMatrix) * vec4(viewPos, 1.0));

	vs_out.TangentLightPos = TBN * modelLightPos;
	vs_out.TangentViewPos = TBN * modelViewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	// Use original vertex position (aPos) to calculate final position
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}