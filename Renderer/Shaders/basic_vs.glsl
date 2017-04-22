#version 330 core

layout (location = 0) in vec3 position;

varying vec3 cols;

void main()
{
	cols = position;
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
}