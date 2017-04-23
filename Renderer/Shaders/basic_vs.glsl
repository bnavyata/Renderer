#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uWorldViewProjection;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPosition;

// light pos
uniform vec3 lightPos;

out vec3 pos;
out vec3 norm;
out vec3 ltPos;
out vec3 cameraPosition;

void main()
{			
	TexCoord = texCoord;
	pos = (uViewMatrix * uWorldMatrix * vec4(position.xyz, 1.0)).xyz;
	ltPos = (uViewMatrix * uWorldMatrix * vec4(lightPos.xyz, 1.0)).xyz;
	norm = (uViewMatrix * uWorldMatrix * vec4(normal.xyz, 1.0)).xyz;
	cameraPosition = (uViewMatrix * uWorldMatrix * vec4(uCameraPosition.xyz, 1.0)).xyz;
	gl_Position = uWorldViewProjection * vec4(position.xyz, 1.0);
}