// Práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>

// Clases para dar orden y limpieza al còdigo
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

// tecla E: Rotar sobre el eje X
// tecla R: Rotar sobre el eje Y
// tecla T: Rotar sobre el eje Z

using std::vector;

// Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0f; // grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

// Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); // recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.5f,
		 0.5f,  0.5f, 0.5f,
		-0.5f,  0.5f, 0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo); // indice 0 en meshList
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
		0, 1, 2,
		1, 3, 2,
		3, 0, 2,
		1, 0, 3
	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,  0.0f,  //0
		 0.5f, -0.5f,  0.0f,  //1
		 0.0f,  0.5f, -0.25f, //2
		 0.0f, -0.5f, -0.5f,  //3
	};
	Mesh* piramidet = new Mesh();
	piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(piramidet); // indice 1 en meshList
}

//función para crear pirámide cuadrangular
// para hacer que cada cara sea de un color se crea un mesh por cada cara
// porque el shader recibe un solo color por dibujo. Asi cada cara puede pintarse distinto
// y el esquema de colores queda dentro de la funcion de la figura y no en el while.
void CrearPiramideCuadrangular()
{
	GLfloat piramidecuadrangular_vertices[] = {
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.0f,  0.5f,  0.0f,
	};

	// Se separan los indices originales, una cara por mesh
	unsigned int idx_frontal[] = { 0, 3, 4 };       //frente
	unsigned int idx_izquierda[] = { 3, 2, 4 };       //izq
	unsigned int idx_trasera[] = { 2, 1, 4 };       //atras
	unsigned int idx_derecha[] = { 1, 0, 4 };       //der
	unsigned int idx_base[] = { 0, 1, 2, 0, 2, 3 }; //abajo1 y abajo2

	Mesh* caraFrontal = new Mesh();
	caraFrontal->CreateMesh(piramidecuadrangular_vertices, idx_frontal, 15, 3);
	meshList.push_back(caraFrontal); // 4: cara frente (roja)

	Mesh* caraIzquierda = new Mesh();
	caraIzquierda->CreateMesh(piramidecuadrangular_vertices, idx_izquierda, 15, 3);
	meshList.push_back(caraIzquierda); // 5: cara izq (verde)

	Mesh* caraTrasera = new Mesh();
	caraTrasera->CreateMesh(piramidecuadrangular_vertices, idx_trasera, 15, 3);
	meshList.push_back(caraTrasera); // 6: cara atras (amarilla)

	Mesh* caraDerecha = new Mesh();
	caraDerecha->CreateMesh(piramidecuadrangular_vertices, idx_derecha, 15, 3);
	meshList.push_back(caraDerecha); // 7: cara der (magenta)

	Mesh* caraBase = new Mesh();
	caraBase->CreateMesh(piramidecuadrangular_vertices, idx_base, 15, 6);
	meshList.push_back(caraBase); // 8: cara de abajo (azul)
}

/*
Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro); // indice 2 en meshList
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono); // indice 3 en meshList
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(36, 1.0f);//índice 2 en MeshList
	CrearCono(25, 1.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índices 4 a 8 en MeshList, una cara por mesh
	CreateShaders();

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 1.5f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);
	// Se usa una matriz para agrupar cada figura
	// guarda la traslacion y las rotaciones del teclado
	// para que todas las piezas de una figura se muevan juntas
	glm::mat4 grupo(1.0);

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	// Se guarda el esquema de colores de la piramide 
	// Como es el mismo arreglo para todas las instancias, las 8 piramides son
	// del mismo color
	glm::vec3 colorCara[5] = {
		glm::vec3(1.0f, 0.0f, 0.0f), // cara frente
		glm::vec3(0.0f, 1.0f, 0.0f), // cara izq
		glm::vec3(1.0f, 1.0f, 0.0f), // cara atras
		glm::vec3(1.0f, 0.0f, 1.0f), // cara der
		glm::vec3(0.0f, 0.0f, 1.0f)  // cara de abajo
	};

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// ==================================================================
		// FIGURA 1: 8 piramides cuadradas pegadas en la base
		// ==================================================================
		// matriz del grupo para mover toda la figura al lado izquierdo,
		// se le aplican las rotaciones de las teclas E, R, T y se reduce de tamano
		grupo = glm::mat4(1.0);
		grupo = glm::translate(grupo, glm::vec3(-2.5f, 0.0f, -2.0f));
		grupo = glm::rotate(grupo, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		grupo = glm::rotate(grupo, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		grupo = glm::rotate(grupo, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		grupo = glm::scale(grupo, glm::vec3(0.6f, 0.6f, 0.6f));

		// Se sube la primera piramide del par de arriba, su base queda en y = 1
		model = glm::translate(grupo, glm::vec3(0.0f, 1.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// Se recorren los 5 meshes de la piramide para poner cada cara con su color
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		// se voltea la segunda piramide en X para que su base toque la anterior
		model = glm::translate(grupo, glm::vec3(0.0f, 0.5f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		// Se repite lo mismo hacia abajo para el par de abajo
		model = glm::translate(grupo, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		model = glm::translate(grupo, glm::vec3(0.0f, -0.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		// Para el par izquierdo se gira 90 en Z y las bases se juntan en x = -1
		model = glm::translate(grupo, glm::vec3(-1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		model = glm::translate(grupo, glm::vec3(-0.5f, 0.0f, 0.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		// Se hace lo mismo del lado derecho pero con el giro contrario, las bases se juntan en x = 1
		model = glm::translate(grupo, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		model = glm::translate(grupo, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colorCara[i]));
			meshList[4 + i]->RenderMesh();
		}

		// ==================================================================
		// FIGURA 2: Cohete
		// ==================================================================
		// Se arma la matriz del grupo para mover el cohete al lado derecho y girarlo con el teclado
		grupo = glm::mat4(1.0);
		grupo = glm::translate(grupo, glm::vec3(2.5f, 0.0f, -2.0f));
		grupo = glm::rotate(grupo, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		grupo = glm::rotate(grupo, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		grupo = glm::rotate(grupo, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		// para e cuerpo se estira un cilindro en Y
		model = glm::translate(grupo, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 2.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// para la punta se pone un cono arriba del cuerpo 
		model = glm::translate(grupo, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		// para las ventanas se usan dos esferas pequenas
		color = glm::vec3(0.15f, 0.25f, 0.85f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = glm::translate(grupo, glm::vec3(0.0f, 0.5f, 0.7f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = glm::translate(grupo, glm::vec3(0.0f, -0.1f, 0.7f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// para los alerones se usan piramides
		// se dibujan las 5 caras del mismo color para que no se vean como la 
		// piramide cuadrada normal
		color = glm::vec3(0.6f, 0.2f, 0.65f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = glm::translate(grupo, glm::vec3(-0.95f, -0.6f, 0.0f));
		model = glm::rotate(model, 135.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 1.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) meshList[4 + i]->RenderMesh();

		model = glm::translate(grupo, glm::vec3(0.95f, -0.6f, 0.0f));
		model = glm::rotate(model, -135.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 1.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		for (int i = 0; i < 5; i++) meshList[4 + i]->RenderMesh();

		// se usa un cubo para la base
		model = glm::translate(grupo, glm::vec3(0.0f, -1.35f, 0.0f));
		model = glm::scale(model, glm::vec3(1.4f, 0.2f, 1.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.68f, 0.42f, 0.25f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// se usa un cilindro para el escape
		model = glm::translate(grupo, glm::vec3(0.0f, -1.70f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// se usa un cono girado 180 en X para el fuego
		model = glm::translate(grupo, glm::vec3(0.0f, -2.55f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}