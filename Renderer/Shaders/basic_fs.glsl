#version 330 core

out vec4 color;

varying vec3 cols;

void main()
{
	color = vec4(abs(cols), 1.0f);
}