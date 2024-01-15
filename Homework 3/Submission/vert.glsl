#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 vertexColor;
in layout(location = 2) vec3 normal;

uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec2 uv;
out vec3 normalObj;
out vec3 vertexPositionWorld;

void main()
{
	uv = vertexColor;
	vec4 v = vec4(position, 1.0);
	vec4 newPosition = modelTransformMatrix * v;
	vec4 cameraposition = viewMatrix * newPosition;
	gl_Position = projectionMatrix * cameraposition;

	vec4 normal_temp = modelTransformMatrix * vec4(normal, 0);
	normalObj = normal_temp.xyz;

	vertexPositionWorld = newPosition.xyz;
}