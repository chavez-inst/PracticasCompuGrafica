#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
	gl_Position = vec4(pos, 1.0);					\n\
}";

//Fragment Shader
// Modificado para recibir un color uniforme en lugar de uno fijo
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
uniform vec4 inputColor;                            \n\
void main()											\n\
{													\n\
	color = inputColor;	 			                \n\
}";

void CrearIniciales()
{
	// 3 letras (CLA) hechas de rectangulos rectos 
	// (2 triangulos por cada una = 54 vertices)
	GLfloat vertices[] = {
		//LETRA C (18 vértices)
		-0.8f, -0.5f, 0.0f,  -0.7f, -0.5f, 0.0f,  -0.8f,  0.5f, 0.0f,
		-0.7f, -0.5f, 0.0f,  -0.7f, 0.5f, 0.0f,  -0.8f,  0.5f, 0.0f,
		-0.7f, 0.4f, 0.0f,  -0.4f, 0.4f, 0.0f,  -0.7f,  0.5f, 0.0f,
		-0.4f, 0.4f, 0.0f,  -0.4f, 0.5f, 0.0f,  -0.7f,  0.5f, 0.0f,
		-0.7f, -0.5f, 0.0f,  -0.4f, -0.5f, 0.0f,  -0.7f, -0.4f, 0.0f,
		-0.4f, -0.5f, 0.0f,  -0.4f, -0.4f, 0.0f,  -0.7f, -0.4f, 0.0f,

		//LETRA L (18 vértices)
		-0.2f, -0.5f, 0.0f,  -0.1f, -0.5f, 0.0f,  -0.2f,  0.5f, 0.0f,
		-0.1f, -0.5f, 0.0f,  -0.1f,  0.5f, 0.0f,  -0.2f,  0.5f, 0.0f,
		-0.1f, -0.5f, 0.0f,  0.2f, -0.5f, 0.0f,  -0.1f, -0.4f, 0.0f,
		 0.2f, -0.5f, 0.0f,  0.2f, -0.4f, 0.0f,  -0.1f, -0.4f, 0.0f,

		 //LETRA A (18 vértices)
		   0.4f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,   0.4f,  0.5f, 0.0f,
		   0.5f, -0.5f, 0.0f,  0.5f,  0.5f, 0.0f,   0.4f,  0.5f, 0.0f,
		   0.7f, -0.5f, 0.0f,  0.8f, -0.5f, 0.0f,   0.7f,  0.5f, 0.0f,
		   0.8f, -0.5f, 0.0f,  0.8f,  0.5f, 0.0f,   0.7f,  0.5f, 0.0f,
		   0.5f, 0.4f, 0.0f,  0.7f,  0.4f, 0.0f,   0.5f, 0.5f, 0.0f,
		   0.7f, 0.4f, 0.0f,  0.7f,  0.5f, 0.0f,   0.5f, 0.5f, 0.0f,
		   0.5f, 0.0f, 0.0f,  0.7f,  0.0f, 0.0f,   0.5f, 0.1f, 0.0f,
		   0.7f, 0.0f, 0.0f,  0.7f,  0.1f, 0.0f,   0.5f, 0.1f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);
}

void CompileShaders() {
	shader = glCreateProgram();
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}
}

int main()
{
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Iniciales CLA - Colores", NULL, NULL);
	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}

	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
	glfwMakeContextCurrent(mainWindow);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, BufferWidth, BufferHeight);

	CrearIniciales();
	CompileShaders();

	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		// FONDO BLANCO (ya no se cambia cada que se repite el while)
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);

		// Se obtiene la ubicación de la variable de color en el shader
		GLint colorLoc = glGetUniformLocation(shader, "inputColor");

		// SE DIBUJA LA LETRA C (Color Rojo) primeros 18 vertices
		glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 18);

		// SE DIBUJA LA LETRA L (Color Verde) los sig 18 vertices
		glUniform4f(colorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 18, 12);

		// SE DIBUJA LA LETRA A (Color Azul) ultimos 18 vertices
		glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 30, 24);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}