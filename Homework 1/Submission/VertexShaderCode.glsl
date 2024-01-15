#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 rotateTransformMatrix;
uniform mat4 scaleTransformMatrix;
uniform mat4 perspectiveTransformMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 out_position = perspectiveTransformMatrix * projectionMatrix * modelTransformMatrix * v * viewMatrix * rotateTransformMatrix * scaleTransformMatrix;
	gl_Position = out_position;	
	theColor = vertexColor;


}
