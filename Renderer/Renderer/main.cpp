#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <freeimage.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void readShaderSource(char* filename, string &out) {
	FILE* fp;
	fopen_s(&fp, filename, "r");

	if (fp == 0) {
		cout << "Could not open shader file: " << filename << endl;
		return;
	}

	out = "";

	char c = NULL;	

	while (c != EOF) {
		c = fgetc(fp);
		if (c!= EOF)
			out += c;
	}
}

GLuint VBO;//vertex buffer object
GLuint shaderProgram;
GLuint VAO;
GLuint EBO;
GLuint uWorldMatrix;
GLuint uViewMatrix;
GLuint uProjectionMatrix;
GLuint uWorldViewProjection;
GLuint uNormalMatrix;
glm::mat4x4 projectionMatrix;
glm::vec3 cameraPosition;
glm::quat cameraRotation;
glm::vec3 lightPos;
GLFWwindow* window;
double deltaTime;
double time;
double lastTime;
double prevX;
double prevY;
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;
GLuint texId;

static const GLfloat vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

GLuint indices[] = {  // Note that we start from 0!
	0, 1, 3,   // First Triangle
	1, 2, 3    // Second Triangle
};

void loadTexture(const string &filename) {
	// Texture not created before. So create new one.

	
	//Texture2D texture2D;
	// dimensions of the texture
	int tex_width, tex_height;
	// raw texture data
	FIBITMAP *texDataPtr;

	const char* filenametemp = filename.c_str();

	FREE_IMAGE_FORMAT image_format = FreeImage_GetFileType(filenametemp, 0);
	texDataPtr = FreeImage_Load(image_format, filenametemp);
	texDataPtr = FreeImage_ConvertTo32Bits(texDataPtr);
	tex_width = FreeImage_GetWidth(texDataPtr);
	tex_height = FreeImage_GetHeight(texDataPtr);
	if (!texDataPtr) {
		std::cerr << "Cannot load texture file " << filenametemp << std::endl;
	}

	GLubyte* final_pixels = new GLubyte[4 * tex_width*tex_height];
	char* temp_pixels = (char*)FreeImage_GetBits(texDataPtr);


	//FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).
	// Switching BGRA -> RGBA
	for (int j = 0; j < tex_width*tex_height; j++) {
		final_pixels[j * 4 + 0] = temp_pixels[j * 4 + 2];
		final_pixels[j * 4 + 1] = temp_pixels[j * 4 + 1];
		final_pixels[j * 4 + 2] = temp_pixels[j * 4 + 0];
		final_pixels[j * 4 + 3] = temp_pixels[j * 4 + 3];
	}

	glGenTextures(1, &texId);
	glActiveTexture(GL_TEXTURE0 + texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, tex_width, tex_height);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, final_pixels);	

}

void Initialize() {

	/* SHADERS */	

	/* for debug log of each compilation success*/
	GLint success;
	GLchar infoLog[512];

	/* vertex shader*/
	GLuint vertexShader;

	// Read shader file into a character array
	string tempStr;
	readShaderSource("Shaders/basic_vs.glsl", tempStr);
	const GLchar* vertexShaderSource = tempStr.c_str();

	// Create a vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Load shader from source into GL_VERTEX_SHADER target represented by vertexShader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile shader
	glCompileShader(vertexShader);

	/* check success for vertex shader*/
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	// Repeat for Fragment shader
	readShaderSource("Shaders/basic_fs.glsl", tempStr);
	const GLchar* fragmentShaderSource = tempStr.c_str();	
	/* fragment shader*/
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	/* check success for fragment shader*/
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/* shader program to link the shaders*/	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::SHADERPROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// MODEL
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	// MATRICES
	
	float fov = 45.0f;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspectRatio = width / height;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	// View -> Projection Matrix	
	projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);	

	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TexCoords
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		//glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// TEXTURES

	loadTexture("Textures/checkerboard.png");
	


	// Set clear color
	glClearColor(0.1, 0.1, 0.1, 1.0);


	// Init light pos
	lightPos = glm::vec3(2.0f, 1.0f, 3.0f);

}

void cameraController() {

	const float speed = 3.0f;
	const float mouseSpeed = 0.1f;

	// Invert vertical look
	bool inverted = false;
	int sign = 1;

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (inverted)
			sign = -1;
		// Compute new orientation
		horizontalAngle += mouseSpeed * float(deltaTime) * float(prevX - xpos);
		verticalAngle += mouseSpeed * float(deltaTime) * float(prevY - ypos) * sign;
	}
	prevX = xpos;
	prevY = ypos;

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	direction = glm::normalize(direction);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	right = glm::normalize(right);

	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPosition += direction * float(deltaTime) * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPosition -= direction  * float(deltaTime) * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPosition += right * float(deltaTime) * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPosition -= right * float(deltaTime) * speed;
	}
	// up
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPosition -= up * float(deltaTime) * speed;
	}
	// down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPosition += up * float(deltaTime) * speed;
	}

	glm::mat4x4 rotMatrix = glm::lookAt(glm::vec3(0), direction, up);
	cameraRotation = glm::quat_cast(rotMatrix);

}

void Draw()
{	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	

	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	// MATRICES
	uWorldMatrix = glGetUniformLocation(shaderProgram, "uWorldMatrix");
	uViewMatrix = glGetUniformLocation(shaderProgram, "uViewMatrix");
	uNormalMatrix = glGetUniformLocation(shaderProgram, "uNormalMatrix");
	uProjectionMatrix = glGetUniformLocation(shaderProgram, "uProjectionMatrix");
	uWorldViewProjection = glGetUniformLocation(shaderProgram, "uWorldViewProjection");
	
	// World Matrix
	glm::mat4x4 worldMatrix = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f,0.0f,-3.0f));
	
	glUniformMatrix4fv(uWorldMatrix, 1, GL_FALSE, &worldMatrix[0][0]);

	cameraController();
	
	// View Matrix
	//cameraPosition.z += 0.001f;
	glm::mat4x4 rotMatrix = glm::mat4_cast(cameraRotation);
	glm::mat4x4 cameraTransMatrix = glm::translate(glm::mat4x4(1.0f), cameraPosition);
	glm::mat4x4 viewMatrix = rotMatrix * glm::inverse(cameraTransMatrix);
	glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, &viewMatrix[0][0]);	
	glUniformMatrix4fv(uProjectionMatrix, 1, GL_FALSE, &projectionMatrix[0][0]);

	// World-View-Projection Matrix
	glm::mat4x4 wvp = projectionMatrix * viewMatrix * worldMatrix;
	glUniformMatrix4fv(uWorldViewProjection, 1, GL_FALSE, &wvp[0][0]);

	// Normal Matrix
	glm::mat3x3 normalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix * worldMatrix));
	glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, &normalMatrix[0][0]);

	GLuint loc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3fv(loc, 1, &lightPos[0]);

	loc = glGetUniformLocation(shaderProgram, "uCameraPosition");
	glUniform3f(loc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	glActiveTexture(GL_TEXTURE0 + texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), texId);

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
}

int main(void)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 600, "Renderer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		return 1;
	}	
	/* setting esc key as a way to exit window*/
	glfwSetKeyCallback(window, key_callback);

	Initialize();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{		
		time = glfwGetTime();
		deltaTime = time - lastTime;
		/* Render here */
		Draw();
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		// Update last time
		lastTime = time;
	}

	glfwTerminate();
	return 0;
}