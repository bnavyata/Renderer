#version 330 core

out vec4 color;

uniform sampler2D myTexture;

in vec2 TexCoord;
in vec3 pos;
in vec3 norm;
in vec3 ltPos;
in vec3 cameraPosition;


uniform mat4 uProjectionMatrix;


void main()
{
	float specFactor = 0.5;

	vec3 lightColor = vec3(0.0,0.0,1.0);
	vec3 viewDir = normalize(cameraPosition - pos);
	vec3 lightDir = normalize(ltPos - pos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float diffuse = max(dot(normalize(norm),lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 finalColor = texture(myTexture, TexCoord).xyz;

	color = vec4(finalColor, 1.0);
}