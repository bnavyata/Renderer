#version 330 core

layout (location = 0) in vec3 position;

varying vec3 cols;

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{			
	vec4 calcPos = uProjectionMatrix * uViewMatrix * uWorldMatrix * vec4(position.xyz, 1.0);
	cols = calcPos.xyz;
	gl_Position = calcPos;
}