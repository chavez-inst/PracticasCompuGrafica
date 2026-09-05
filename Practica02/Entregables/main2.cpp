// Práctica 2: indices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
// glm
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
// clases para dar orden y limpieza al código
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

const float toRadians = 3.14159265f / 180.0;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Fragment shader base que recibe vColor del vertex shader
static const char* fShader = "shaders/shader.frag";

// Se dibuja la figura usando el shader y matriz de modelo correspondientes
void DibujarFigura(int shaderIndex, Mesh* figura, glm::mat4 modelMatrix, glm::mat4 projMatrix) {
	shaderList[shaderIndex].useShader();
	GLuint uModel = shaderList[shaderIndex].getModelLocation();
	GLuint uProj = shaderList[shaderIndex].getProjectLocation();
	glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projMatrix));
	figura->RenderMesh();
}

// Se crea la pirámide triangular normal
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,  1,3,2,  3,0,2,  1,0,3
	};
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, -0.25f,
		 0.0f, -0.5f, -0.5f,
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

// Se crea un cubo 
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2,  2, 3, 0,  1, 5, 6,  6, 2, 1,  7, 6, 5,  5, 4, 7,
		4, 0, 3,  3, 7, 4,  4, 5, 1,  1, 0, 4,  3, 2, 6,  6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Se crea la piramide del archivo de classroom
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,  3,2,4,  2,1,4,  1,0,4,  0,1,2,  0,2,4
	};
	GLfloat piramidecuadrangular_vertices[] = {
		 0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f, 0.5f,
		 0.0f, 0.5f, 0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

// Se cargan y compilan los shaders de colores
void CreateShaders()
{
	Shader* shaderRojo = new Shader();
	shaderRojo->CreateFromFiles("shaders/shaderrojo.vert", fShader);
	shaderList.push_back(*shaderRojo); // Index 0

	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles("shaders/shaderverde.vert", fShader);
	shaderList.push_back(*shaderVerde); // Index 1

	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles("shaders/shaderazul.vert", fShader);
	shaderList.push_back(*shaderAzul); // Index 2

	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles("shaders/shadercafe.vert", fShader);
	shaderList.push_back(*shaderCafe); // Index 3

	Shader* shaderMagenta = new Shader();
	shaderMagenta->CreateFromFiles("shaders/shadermagenta.vert", fShader);
	shaderList.push_back(*shaderMagenta); // Index 4

	Shader* shaderAmarillo = new Shader();
	shaderAmarillo->CreateFromFiles("shaders/shaderamarillo.vert", fShader);
	shaderList.push_back(*shaderAmarillo); // Index 5
}

int main()
{
	mainWindow = Window(1280, 720);
	mainWindow.Initialise();

	CreaPiramide();              // meshList[0] = Pirámide Triangular
	CrearCubo();                 // meshList[1] = Cubo
	CrearPiramideCuadrangular(); // meshList[2] = Pirámide Cuadrangular
	CreateShaders();

	const int ROJO = 0;
	const int VERDE = 1;
	const int AZUL = 2;
	const int CAFE = 3;
	const int MAGENTA = 4;
	const int AMARILLO = 5;

	glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -2.25f, 2.25f, 0.1f, 100.0f);
	glm::mat4 model(1.0);

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();

		glClearColor(1.0f, 0.85f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Se dibuja el piso con un cubo escalado
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.9f, -2.0f));
		model = glm::scale(model, glm::vec3(10.0f, 0.2f, 1.0f));
		DibujarFigura(CAFE, meshList[1], model, projection);

		// =========================================================================
		// Torre de la izq
		// se hacen piramides triangulares apuntando hacia abajo
		// =========================================================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.5f, -1.4f, -1.9f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.8f, 1.0f));
		DibujarFigura(VERDE, meshList[0], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.5f, -0.6f, -1.9f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.8f, 1.0f));
		DibujarFigura(ROJO, meshList[0], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.5f, 0.2f, -1.9f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.8f, 1.0f));
		DibujarFigura(AMARILLO, meshList[0], model, projection);

		// los postes se hacen usando cubos alargados
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, -0.6f, -2.0f));
		model = glm::scale(model, glm::vec3(0.12f, 2.4f, 1.0f));
		DibujarFigura(CAFE, meshList[1], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -0.6f, -2.0f));
		model = glm::scale(model, glm::vec3(0.12f, 2.4f, 1.0f));
		DibujarFigura(CAFE, meshList[1], model, projection);


		// =========================================================================
		// Figura del centro
		// se rotan las piramides cuadradas 90° para los cuadros
		// se ponen en Z = -3.0 para que vayan atras
		// =========================================================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.45f, -1.35f, -3.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.9f, 1.0f));
		DibujarFigura(MAGENTA, meshList[2], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.45f, -1.35f, -3.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.9f, 1.0f));
		DibujarFigura(VERDE, meshList[2], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.45f, -0.45f, -3.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.9f, 1.0f));
		DibujarFigura(AMARILLO, meshList[2], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.45f, -0.45f, -3.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.9f, 1.0f));
		DibujarFigura(ROJO, meshList[2], model, projection);

		// los rombos se hacen rotando cubos a 45 grados al frente
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, -1.9f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.273f, 1.273f, 1.0f));
		DibujarFigura(AZUL, meshList[1], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, -1.8f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 1.0f));
		DibujarFigura(CAFE, meshList[1], model, projection);


		// =========================================================================
		// Trifuerza derecha
		// se apilan 4 piramides cuacuadradas 
		// =========================================================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.3f, -1.3f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		DibujarFigura(VERDE, meshList[2], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.3f, -1.3f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		DibujarFigura(ROJO, meshList[2], model, projection);

		// se usa una piramide invertida en el centro
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.8f, -1.3f, -1.9f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		DibujarFigura(AMARILLO, meshList[2], model, projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.8f, -0.3f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		DibujarFigura(MAGENTA, meshList[2], model, projection);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}