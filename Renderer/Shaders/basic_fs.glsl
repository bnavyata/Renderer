#version 330 core

out vec4 color;

uniform sampler2D myTexture;

in vec2 TexCoord;
in vec3 pos;
in vec3 norm;
in vec3 ltPos;
in vec3 cameraPosition;

void main()
{
	float specFactor = 0.5;

	vec3 lightColor = vec3(0.0,0.0,1.0);
	vec3 viewDir = normalize(cameraPosition - pos);
	vec3 lightDir = normalize(ltPos - pos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float diffuse = max(dot(norm,lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	//vec3 finalColor = texture(myTexture, TexCoord).xyz * diffuse * lightColor;
	vec3 purple = vec3(0.7,0.4,0.9);
	vec3 finalColor = purple * ( diffuse + spec);

	color = vec4(finalColor, 1.0);
}