#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <string>
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

GLfloat vertices[] = {
	0.5f,  0.5f, 0.0f,  // Top Right
	0.5f, -0.5f, 0.0f,  // Bottom Right
	-0.5f, -0.5f, 0.0f,  // Bottom Left
	-0.5f,  0.5f, 0.0f   // Top Left 
};

GLuint indices[] = {  // Note that we start from 0!
	0, 1, 3,   // First Triangle
	1, 2, 3    // Second Triangle
};

void Initialize() {	

	/* SHADERS */	

	/* for debug log of each compilation success*/
	GLint success;
	GLchar infoLog[512];

	/* vertex shader*/
	GLuint vertexShader;

	string tempStr;
	readShaderSource("Shaders/basic_vs.glsl", tempStr);

	const GLchar* vertexShaderSource = tempStr.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	/* check success for vertex shader*/
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

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

	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

	glBindVertexArray(0);

}

void Draw()
{
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);	
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int main(void)
{
	/*creates a window*/
	GLFWwindow* window;

	

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
		/* Render here */
		

		Draw();
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}