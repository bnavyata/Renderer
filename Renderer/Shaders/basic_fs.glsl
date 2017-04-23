#version 330 core

out vec4 color;

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

varying vec3 cols;

void main()
{
	color = vec4(abs(cols), 1.0f);
}