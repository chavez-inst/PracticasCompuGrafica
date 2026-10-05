/*
Práctica 6: Texturizado
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION


#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz

std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture holocronTexture;

Model Kitt_M;
Model Llanta_M;
Model Dado_M;
Model Holocron_M;
Model Avion_M;		// Ejercicio 3: avion estilo Cars

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";





void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};



	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};


	GLfloat cubo_vertices[] = {
		//  x      y      z      S      T      NX     NY     NZ
		// front (Separatistas)
		-0.5f, -0.5f,  0.5f,  0.51f, 0.26f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,  0.74f, 0.26f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f,  0.5f,  0.74f, 0.49f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,  0.51f, 0.49f,  0.0f,  0.0f, -1.0f,

		// right (Republica)
		 0.5f, -0.5f,  0.5f,  0.76f, 0.26f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.99f, 0.26f, -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.99f, 0.49f, -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.76f, 0.49f, -1.0f,  0.0f,  0.0f,

		 // back (Sith)
		 // Aqui S va al reves para que el logo no salga como espejo
		 -0.5f, -0.5f, -0.5f,  0.24f, 0.26f,  0.0f,  0.0f,  1.0f,
		  0.5f, -0.5f, -0.5f,  0.01f, 0.26f,  0.0f,  0.0f,  1.0f,
		  0.5f,  0.5f, -0.5f,  0.01f, 0.49f,  0.0f,  0.0f,  1.0f,
		 -0.5f,  0.5f, -0.5f,  0.24f, 0.49f,  0.0f,  0.0f,  1.0f,

		 // left (Primera Orden)
		 -0.5f, -0.5f, -0.5f,  0.26f, 0.26f,  1.0f,  0.0f,  0.0f,
		 -0.5f, -0.5f,  0.5f,  0.49f, 0.26f,  1.0f,  0.0f,  0.0f,
		 -0.5f,  0.5f,  0.5f,  0.49f, 0.49f,  1.0f,  0.0f,  0.0f,
		 -0.5f,  0.5f, -0.5f,  0.26f, 0.49f,  1.0f,  0.0f,  0.0f,

		 // bottom (Alianza Rebelde)
		 -0.5f, -0.5f,  0.5f,  0.51f, 0.24f,  0.0f,  1.0f,  0.0f,
		  0.5f, -0.5f,  0.5f,  0.74f, 0.24f,  0.0f,  1.0f,  0.0f,
		  0.5f, -0.5f, -0.5f,  0.74f, 0.01f,  0.0f,  1.0f,  0.0f,
		 -0.5f, -0.5f, -0.5f,  0.51f, 0.01f,  0.0f,  1.0f,  0.0f,

		 // top (Nueva Republica)
		 -0.5f,  0.5f,  0.5f,  0.51f, 0.51f,  0.0f, -1.0f,  0.0f,
		  0.5f,  0.5f,  0.5f,  0.74f, 0.51f,  0.0f, -1.0f,  0.0f,
		  0.5f,  0.5f, -0.5f,  0.74f, 0.74f,  0.0f, -1.0f,  0.0f,
		 -0.5f,  0.5f, -0.5f,  0.51f, 0.74f,  0.0f, -1.0f,  0.0f,
	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);
}


void CrearHolocron()
{
	unsigned int holocron_indices[] = {
		0, 1, 2,  0, 2, 3,	// -x Primera Orden
		4, 5, 6,  4, 6, 7,	// +z Separatistas
		8, 9, 10,  8, 10, 11,	// -z Sith
		12, 13, 14,  12, 14, 15,	// -y Alianza Rebelde
		16, 17, 18,  16, 18, 19,	// +x Republica
		20, 21, 22,  20, 22, 23,	// +y Nueva Republica
		24, 25, 26,	// triangulo (casilla negra)
		27, 28, 29,	// triangulo (casilla negra)
		30, 31, 32,	// triangulo (casilla negra)
		33, 34, 35,	// triangulo (casilla negra)
		36, 37, 38,	// triangulo (casilla negra)
		39, 40, 41,	// triangulo (casilla negra)
		42, 43, 44,	// triangulo (casilla negra)
		45, 46, 47,	// triangulo (casilla negra)
	};

	GLfloat holocron_vertices[] = {
		//x		y		z		S	T		NX	NY	NZ
		// -x Primera Orden
		-2.646786f, -0.139047f, 2.493757f,	0.4850f, 0.3723f,	0.9992f, -0.0137f, 0.0385f,	//0
		-2.451372f, 2.421759f, 0.077538f,	0.3779f, 0.4850f,	0.9992f, -0.0137f, 0.0385f,	//1
		-2.453980f, -0.063388f, -2.477451f,	0.2650f, 0.3756f,	0.9992f, -0.0137f, 0.0385f,	//2
		-2.517274f, -2.579095f, 0.002115f,	0.3747f, 0.2650f,	0.9992f, -0.0137f, 0.0385f,	//3
		// +z Separatistas
		2.593003f, -0.085222f, 2.505755f,	0.7350f, 0.3757f,	0.0023f, -0.0028f, -1.0000f,	//4
		0.051152f, 2.399781f, 2.545043f,	0.6283f, 0.4850f,	0.0023f, -0.0028f, -1.0000f,	//5
		-2.646786f, -0.139047f, 2.493757f,	0.5150f, 0.3733f,	0.0023f, -0.0028f, -1.0000f,	//6
		-0.140696f, -2.601934f, 2.558561f,	0.6202f, 0.2650f,	0.0023f, -0.0028f, -1.0000f,	//7
		// -z Sith
		2.534944f, -0.061146f, -2.580280f,	0.0150f, 0.3739f,	0.0206f, -0.0016f, 0.9998f,	//8
		0.081432f, -2.513204f, -2.426005f,	0.1233f, 0.2650f,	0.0206f, -0.0016f, 0.9998f,	//9
		-2.453980f, -0.063388f, -2.477451f,	0.2350f, 0.3738f,	0.0206f, -0.0016f, 0.9998f,	//10
		0.049649f, 2.440128f, -2.417217f,	0.1247f, 0.4850f,	0.0206f, -0.0016f, 0.9998f,	//11
		// -y Alianza Rebelde
		-0.140696f, -2.601934f, 2.558561f,	0.6165f, 0.2350f,	-0.0049f, 0.9998f, 0.0176f,	//12
		-2.517274f, -2.579095f, 0.002115f,	0.5150f, 0.1222f,	-0.0049f, 0.9998f, 0.0176f,	//13
		0.081432f, -2.513204f, -2.426005f,	0.6260f, 0.0150f,	-0.0049f, 0.9998f, 0.0176f,	//14
		2.632201f, -2.553892f, 0.007226f,	0.7350f, 0.1224f,	-0.0049f, 0.9998f, 0.0176f,	//15
		// +x Republica
		2.521206f, 2.448241f, 0.063388f,	0.8707f, 0.4850f,	-0.9997f, -0.0223f, 0.0113f,	//16
		2.593003f, -0.085222f, 2.505755f,	0.7650f, 0.3736f,	-0.9997f, -0.0223f, 0.0113f,	//17
		2.632201f, -2.553892f, 0.007226f,	0.8730f, 0.2650f,	-0.9997f, -0.0223f, 0.0113f,	//18
		2.534944f, -0.061146f, -2.580280f,	0.9850f, 0.3746f,	-0.9997f, -0.0223f, 0.0113f,	//19
		// +y Nueva Republica
		-2.451372f, 2.421759f, 0.077538f,	0.5150f, 0.6244f,	0.0053f, -1.0000f, -0.0081f,	//20
		0.051152f, 2.399781f, 2.545043f,	0.6257f, 0.5150f,	0.0053f, -1.0000f, -0.0081f,	//21
		2.521206f, 2.448241f, 0.063388f,	0.7350f, 0.6250f,	0.0053f, -1.0000f, -0.0081f,	//22
		0.049649f, 2.440128f, -2.417217f,	0.6257f, 0.7350f,	0.0053f, -1.0000f, -0.0081f,	//23
		// triangulo (casilla negra)
		-2.517274f, -2.579095f, 0.002115f,	0.0500f, 0.8000f,	0.5482f, 0.5800f, 0.6025f,	//24
		-2.453980f, -0.063388f, -2.477451f,	0.2000f, 0.8000f,	0.5482f, 0.5800f, 0.6025f,	//25
		0.081432f, -2.513204f, -2.426005f,	0.1250f, 0.9500f,	0.5482f, 0.5800f, 0.6025f,	//26
		// triangulo (casilla negra)
		2.521206f, 2.448241f, 0.063388f,	0.0500f, 0.8000f,	-0.5661f, -0.5994f, 0.5660f,	//27
		2.534944f, -0.061146f, -2.580280f,	0.2000f, 0.8000f,	-0.5661f, -0.5994f, 0.5660f,	//28
		0.049649f, 2.440128f, -2.417217f,	0.1250f, 0.9500f,	-0.5661f, -0.5994f, 0.5660f,	//29
		// triangulo (casilla negra)
		-2.451372f, 2.421759f, 0.077538f,	0.0500f, 0.8000f,	0.5737f, -0.5874f, 0.5708f,	//30
		0.049649f, 2.440128f, -2.417217f,	0.2000f, 0.8000f,	0.5737f, -0.5874f, 0.5708f,	//31
		-2.453980f, -0.063388f, -2.477451f,	0.1250f, 0.9500f,	0.5737f, -0.5874f, 0.5708f,	//32
		// triangulo (casilla negra)
		-2.646786f, -0.139047f, 2.493757f,	0.0500f, 0.8000f,	0.5649f, -0.5887f, -0.5782f,	//33
		0.051152f, 2.399781f, 2.545043f,	0.2000f, 0.8000f,	0.5649f, -0.5887f, -0.5782f,	//34
		-2.451372f, 2.421759f, 0.077538f,	0.1250f, 0.9500f,	0.5649f, -0.5887f, -0.5782f,	//35
		// triangulo (casilla negra)
		0.051152f, 2.399781f, 2.545043f,	0.0500f, 0.8000f,	-0.5732f, -0.5771f, -0.5818f,	//36
		2.593003f, -0.085222f, 2.505755f,	0.2000f, 0.8000f,	-0.5732f, -0.5771f, -0.5818f,	//37
		2.521206f, 2.448241f, 0.063388f,	0.1250f, 0.9500f,	-0.5732f, -0.5771f, -0.5818f,	//38
		// triangulo (casilla negra)
		-0.140696f, -2.601934f, 2.558561f,	0.0500f, 0.8000f,	0.5940f, 0.5900f, -0.5469f,	//39
		-2.646786f, -0.139047f, 2.493757f,	0.2000f, 0.8000f,	0.5940f, 0.5900f, -0.5469f,	//40
		-2.517274f, -2.579095f, 0.002115f,	0.1250f, 0.9500f,	0.5940f, 0.5900f, -0.5469f,	//41
		// triangulo (casilla negra)
		2.593003f, -0.085222f, 2.505755f,	0.0500f, 0.8000f,	-0.5530f, 0.5883f, -0.5900f,	//42
		-0.140696f, -2.601934f, 2.558561f,	0.2000f, 0.8000f,	-0.5530f, 0.5883f, -0.5900f,	//43
		2.632201f, -2.553892f, 0.007226f,	0.1250f, 0.9500f,	-0.5530f, 0.5883f, -0.5900f,	//44
		// triangulo (casilla negra)
		2.632201f, -2.553892f, 0.007226f,	0.0500f, 0.8000f,	-0.5524f, 0.5898f, 0.5890f,	//45
		0.081432f, -2.513204f, -2.426005f,	0.2000f, 0.8000f,	-0.5524f, 0.5898f, 0.5890f,	//46
		2.534944f, -0.061146f, -2.580280f,	0.1250f, 0.9500f,	-0.5524f, 0.5898f, 0.5890f,	//47
	};

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(holocron_vertices, holocron_indices, 384, 60);
	meshListModel.push_back(holocron);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	// Textura con los 6 logos (es JPG no tiene canal alfa)
	dadoTexture = Texture("Textures/LogosSWOptimizados.jpg");
	dadoTexture.LoadTexture();   // LoadTexture en vez de LoadTextureA porque JPG no tiene alfa
	holocronTexture = Texture("Textures/holocronunwrap.png");
	holocronTexture.LoadTextureA();

	Holocron_M = Model();
	Holocron_M.LoadModel("Models/holocron_texturizado.obj");

	Avion_M = Model();
	Avion_M.LoadModel("Models/avion_cars.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshListModel[2]->RenderMeshModel();




		//Dado de Opengl
		//Ejercicio 1: Texturizar su dado con la imagen ya optimizada por ustedes con logos de star wars
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();

		//Ejercicio 2:Importar el cubo texturizado en el programa de modelado con 
		//la imagen ya optimizada por ustedes

		/*
		//Dado importado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 3.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado_M.RenderModel();
		*/




		/*Reporte de práctica :

		Ejercicio 1: Crear o modificar el holocron y texturizarlo por medio de código
		Ejercicio 2: Importar el modelo del holocron texturizardo en el programa de modelado
		Ejercicio 3: Importar un modelo de avión con con la textura de la cara del personaje de la imagen del previo:
		Vidrio fonrtal: OJOS
		Frente del avión: Nariz y Sonrisa
		Alas: Logos del universo del personaje

		*/


		//Holocrones
		//Ejercicio 1: holocron por codigo, texturizado con LogosSWOptimizados.jpg
		color = glm::vec3(1.0f, 1.0f, 1.0f);//blanco para no alterar el color de los logos
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.5f, 4.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshListModel[5]->RenderMeshModel();


		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.5f, 2.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Holocron_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avion_M.RenderModel();





		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/