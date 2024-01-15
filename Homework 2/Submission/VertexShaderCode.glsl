#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV; 
layout(location = 3) in vec2 vertexUV2; 
layout(location = 2) in vec3 normal;
layout(location = 1) in vec3 vertexColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 TransformMatrix;
uniform mat4 RotationMatrix;
uniform mat4 ScalingMatrix;

out vec2 UV;
out vec2 UV2; // Added this line
out vec3 theColor;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	vec4 worldPosition = TransformMatrix * vec4(position, 1.0);
	vec4 out_position = projectionMatrix * worldPosition * viewMatrix * RotationMatrix * ScalingMatrix;
	gl_Position = out_position;	
	theColor = vertexColor;
	UV = vertexUV;
	UV2 = vertexUV2; // Added this line

	FragPos = vec3(worldPosition);
	Normal = mat3(transpose(inverse(TransformMatrix))) * normal;
}