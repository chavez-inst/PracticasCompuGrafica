/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
bool avanza;

float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
float movCoche = 0.0f;
float movOffset = 0.01f;

// variables para que el rover vaya de una orilla del piso a la otra
// como el piso llega a 30, se le resta lo que mide medio rover para marcar el limite
float limiteRover = 27.0f;
float giroLlanta = 0.0f;
// lo que gira la llanta en cada frame, para que se vea que rueda y no que se resbala
float giroLlantaOffset = 17.0f;
float escalaRover = 0.4f;

// variables del avion, que da la vuelta con una recta, una curva, otra recta y otra curva
glm::vec3 posAvion = glm::vec3(-15.0f, 16.0f, 8.6f);
// empieza rotado 90 grados para que la nariz apunte hacia X positivo
float anguloAvion = 90.0f;
// indica en que parte de la vuelta va el avion, 0 y 2 son las rectas y 1 y 3 son las curvas
int estadoAvion = 0;
float velocidadAvion = 0.15f;
// lo que gira el avion en cada frame cuando va en curva
float giroAvionOffset = 1.0f;
// hasta donde llega el avion antes de empezar a dar la curva
float limiteAvion = 15.0f;
float giroHelice = 0.0f;
float giroHeliceOffset = 20.0f;

// medidas de la pista
// la nave tambien las usa para saber donde termina cada parte
float alturaPista = -1.9f;      // un poco arriba del piso para que no se encimen
float anchoPista = 4.0f;
float radioCurva = 8.0f;
// la pista tiene dos rectas, una con rampas y otra plana
float xInicioPista = -12.0f;
float xFinPista = 12.0f;
float zRectaRampas = 24.0f;
float zRectaPlana = 8.0f;       // asi las curvas unen las dos rectas
float largoRampa = 8.0f;
float alturaRampa = 3.0f;

// variables de la nave, que da vueltas a toda la pista
// la nave va un poco arriba de la pista
float alturaNave = 0.6f;
glm::vec3 posNave = glm::vec3(-12.0f, -1.3f, 24.0f);
// empieza rotada 90 grados para que la nariz apunte hacia X positivo
float anguloNave = 90.0f;
// cuanto sube o baja la nariz en las rampas
float inclinacionNave = 0.0f;
// indica en que parte del circuito va la nave
// 0 es la rampa de subida, 1 la recta de arriba, 2 la rampa de bajada,
// 3 la primera curva, 4 la recta plana y 5 la segunda curva
int estadoNave = 0;
float velocidadNave = 0.15f;
float escalaNave = 0.3f;

// variables del satelite, que da vueltas arriba de la pista
glm::vec3 centroOrbita = glm::vec3(0.0f, 10.0f, 16.0f);
float radioOrbita = 16.0f;
float anguloSatelite = 0.0f;
// lo que avanza el satelite en cada frame
float anguloSateliteOffset = 0.3f;
float escalaSatelite = 1.5f;
// los paneles se inclinan de un lado a otro
// antes esto se hacia con el teclado
float giroPaneles = 0.0f;
float giroPanelesOffset = 0.5f;
float limitePaneles = 40.0f;
bool panelesSubiendo = true;
// la antena gira todo el tiempo
float giroAntena = 0.0f;
float giroAntenaOffset = 1.5f;

Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture pistaTexture;



// rover con jerarquia
Model roverCuerpo;
Model baseBrazo;
Model extBrazo1;
Model extBrazo2;
Model basePinza;
Model pinza;
Model brazoRuedaFIzq;
Model ejeRuedaFIzq;
Model baseRuedaFDer;
Model ejeRuedaFDer;
Model llantaRuedaFIzq;
Model llantaRuedaFDer;
Model brazoRuedaCDer;
Model brazoRuedaTDer;
Model llantaRuedaCDer;

// avion con jerarquia, las turbinas van pegadas al cuerpo y las helices a cada turbina
Model Avion_M;
Model Turbina_M;
Model Helice_M;

Model Nave_M;

// satelite con jerarquia, los paneles y la antena van pegados al cuerpo
Model SateliteCuerpo_M;
Model SatelitePanel_M;
Model SateliteAntena_M;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


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
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

	// pedazo de pista, se escala y se gira
	// para armar las rectas, las rampas y las curvas
	unsigned int pistaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat pistaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	MeshModel* obj8 = new MeshModel();
	obj8->CreateMeshModel(pistaVertices, pistaIndices, 32, 6);
	meshList.push_back(obj8); // pista

	// triangulo para tapar los lados de las rampas
	unsigned int paredRampaIndices[] = {
	   0, 1, 2,
	};

	GLfloat paredRampaVertices[] = {
		-0.5f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 1.0f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
	};

	MeshModel* obj9 = new MeshModel();
	obj9->CreateMeshModel(paredRampaVertices, paredRampaIndices, 24, 3);
	meshList.push_back(obj9); // lado de la rampa

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();
	pistaTexture = Texture("Textures/pista.png");
	pistaTexture.LoadTextureA();

	// rover con jerarquia
	std::string carpetaRover = "Models/Rover/";
	roverCuerpo = Model();
	roverCuerpo.LoadModel(carpetaRover + "rover_cuerpo.obj");
	baseBrazo = Model();
	baseBrazo.LoadModel(carpetaRover + "rover_brazo_base.obj");
	extBrazo1 = Model();
	extBrazo1.LoadModel(carpetaRover + "rover_brazo_segmento1.obj");
	extBrazo2 = Model();
	extBrazo2.LoadModel(carpetaRover + "rover_brazo_segmento2.obj");
	basePinza = Model();
	basePinza.LoadModel(carpetaRover + "rover_pinza_base.obj");
	pinza = Model();
	pinza.LoadModel(carpetaRover + "rover_pinza.obj");
	brazoRuedaFIzq = Model();
	brazoRuedaFIzq.LoadModel(carpetaRover + "rover_suspension_delantera_izq.obj");
	ejeRuedaFIzq = Model();
	ejeRuedaFIzq.LoadModel(carpetaRover + "rover_eje_rueda_delantera_izq.obj");
	baseRuedaFDer = Model();
	baseRuedaFDer.LoadModel(carpetaRover + "rover_soporte_rueda_delantera_der.obj");
	ejeRuedaFDer = Model();
	ejeRuedaFDer.LoadModel(carpetaRover + "rover_eje_rueda_delantera_der.obj");
	// cada llanta delantera trae su rin para que gire junto con ella
	llantaRuedaFIzq = Model();
	llantaRuedaFIzq.LoadModel(carpetaRover + "rover_llanta_delantera_izq.obj");
	llantaRuedaFDer = Model();
	llantaRuedaFDer.LoadModel(carpetaRover + "rover_llanta_delantera_der.obj");
	brazoRuedaCDer = Model();
	brazoRuedaCDer.LoadModel(carpetaRover + "rover_suspension_central_der.obj");
	brazoRuedaTDer = Model();
	brazoRuedaTDer.LoadModel(carpetaRover + "rover_suspension_trasera_der.obj");
	llantaRuedaCDer = Model();
	llantaRuedaCDer.LoadModel(carpetaRover + "rover_llanta_central_der.obj");

	// se carga el avion de la practica 6 y aparte sus turbinas y helices
	Avion_M = Model();
	Avion_M.LoadModel("Models/Avion/avion_cars.obj");
	Turbina_M = Model();
	Turbina_M.LoadModel("Models/Avion/avion_turbina.obj");
	Helice_M = Model();
	Helice_M.LoadModel("Models/Avion/avion_helice.obj");

	// nave de la practica 8
	Nave_M = Model();
	Nave_M.LoadModel("Models/Nave/nave.obj");

	// satelite separado en piezas para mover cada una
	SateliteCuerpo_M = Model();
	SateliteCuerpo_M.LoadModel("Models/Satelite/satelite_cuerpo.obj");
	SatelitePanel_M = Model();
	SatelitePanel_M.LoadModel("Models/Satelite/satelite_panel.obj");
	SateliteAntena_M = Model();
	SateliteAntena_M.LoadModel("Models/Satelite/satelite_antena.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.15f;
	avanza = true;

	// angulo de las rampas y lo que mide cada rampa ya inclinada
	float inclinacionRampa = atan(alturaRampa / largoRampa) / toRadians;
	float largoRampaInclinada = sqrt(largoRampa * largoRampa + alturaRampa * alturaRampa);
	// lo que sube o baja la nave por cada unidad que avanza en una rampa
	float pendienteRampa = alturaRampa / largoRampa;
	// lo que gira la nave en cada frame en las curvas
	// se saca del radio para que siga la curva de la pista
	float giroNaveOffset = (velocidadNave / radioCurva) / toRadians;
	// cuantos pedazos de pista se usan para dibujar cada curva
	int piezasCurva = 24;


	glm::vec3 lowerLight(0.0f,0.0f,0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

		//rover avanza hasta una orilla y retrocede hasta la otra
		// el frente del rover apunta hacia X negativo, por eso al avanzar se le resta
		if (avanza)
		{
			movCoche -= movOffset * deltaTime;
			giroLlanta += giroLlantaOffset * deltaTime;
			if (movCoche < -limiteRover)
				avanza = false;
		}
		else
		{
			movCoche += movOffset * deltaTime;
			giroLlanta -= giroLlantaOffset * deltaTime;
			if (movCoche > limiteRover)
				avanza = true;
		}

		// el avion siempre avanza hacia donde apunta su nariz, asi nunca va de reversa
		if (estadoAvion == 0)
		{
			// el avion va derecho hacia X positivo
			if (posAvion.x > limiteAvion)
				estadoAvion = 1;
		}
		else if (estadoAvion == 1)
		{
			// el avion da la curva hasta quedar de regreso
			anguloAvion += giroAvionOffset * deltaTime;
			if (anguloAvion >= 270.0f)
			{
				anguloAvion = 270.0f;
				estadoAvion = 2;
			}
		}
		else if (estadoAvion == 2)
		{
			// el avion va derecho hacia X negativo
			if (posAvion.x < -limiteAvion)
				estadoAvion = 3;
		}
		else if (estadoAvion == 3)
		{
			// el avion da la otra curva para volver a empezar la vuelta
			anguloAvion += giroAvionOffset * deltaTime;
			if (anguloAvion >= 450.0f)
			{
				anguloAvion = 90.0f;
				estadoAvion = 0;
			}
		}
		// con el angulo se saca hacia donde apunta la nariz y el avion avanza en esa direccion
		posAvion.x += sin(anguloAvion * toRadians) * velocidadAvion * deltaTime;
		posAvion.z += cos(anguloAvion * toRadians) * velocidadAvion * deltaTime;

		giroHelice += giroHeliceOffset * deltaTime;

		// la nave siempre avanza hacia donde apunta su nariz
		// y en las rampas tambien sube o baja
		if (estadoNave == 0)
		{
			// rampa de subida
			inclinacionNave = inclinacionRampa;
			posNave.y += pendienteRampa * velocidadNave * deltaTime;
			if (posNave.x > xInicioPista + largoRampa)
			{
				posNave.y = alturaPista + alturaRampa + alturaNave;
				estadoNave = 1;
			}
		}
		else if (estadoNave == 1)
		{
			// recta de arriba
			inclinacionNave = 0.0f;
			if (posNave.x > xFinPista - largoRampa)
				estadoNave = 2;
		}
		else if (estadoNave == 2)
		{
			// rampa de bajada
			inclinacionNave = -inclinacionRampa;
			posNave.y -= pendienteRampa * velocidadNave * deltaTime;
			if (posNave.x > xFinPista)
			{
				posNave.y = alturaPista + alturaNave;
				inclinacionNave = 0.0f;
				estadoNave = 3;
			}
		}
		else if (estadoNave == 3)
		{
			// primera curva, gira hasta quedar de regreso
			anguloNave += giroNaveOffset * deltaTime;
			if (anguloNave >= 270.0f)
			{
				// se acomoda justo en la recta
				// para que no se vaya desviando con cada vuelta
				anguloNave = 270.0f;
				posNave.z = zRectaPlana;
				estadoNave = 4;
			}
		}
		else if (estadoNave == 4)
		{
			// recta plana
			if (posNave.x < xInicioPista)
				estadoNave = 5;
		}
		else if (estadoNave == 5)
		{
			// segunda curva, gira hasta volver al inicio de la rampa
			anguloNave += giroNaveOffset * deltaTime;
			if (anguloNave >= 450.0f)
			{
				anguloNave = 90.0f;
				posNave.z = zRectaRampas;
				estadoNave = 0;
			}
		}
		// la nave avanza hacia donde apunta su nariz
		posNave.x += sin(anguloNave * toRadians) * velocidadNave * deltaTime;
		posNave.z += cos(anguloNave * toRadians) * velocidadNave * deltaTime;

		// el satelite avanza en su vuelta y sus piezas se mueven solas
		anguloSatelite += anguloSateliteOffset * deltaTime;
		if (anguloSatelite > 360.0f)
			anguloSatelite -= 360.0f;
		if (panelesSubiendo)
		{
			giroPaneles += giroPanelesOffset * deltaTime;
			if (giroPaneles > limitePaneles)
				panelesSubiendo = false;
		}
		else
		{
			giroPaneles -= giroPanelesOffset * deltaTime;
			if (giroPaneles < -limitePaneles)
				panelesSubiendo = true;
		}
		giroAntena += giroAntenaOffset * deltaTime;

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
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model= glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		//piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		// se hizo mas chico el piso para que el rover si alcance a llegar a las orillas
		model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMeshModel();



		//Instancia del rover
		// cuerpo principal del rover
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, -0.82f, 2.0f));
		model = glm::scale(model, glm::vec3(escalaRover, escalaRover, escalaRover));
		modelaux2 = model;    // guarda el cuerpo para colgar de el cada rueda y parte
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		roverCuerpo.RenderModel();

		// brazo del rover
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-2.6f, 1.2f, 1.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseBrazo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		extBrazo1.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.4f, 2.7f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		extBrazo2.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(3.0f, 2.9f, -0.1f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		basePinza.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.45f, 0.4f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pinza.RenderModel();

		// rueda frontal izquierda
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-2.0f, 1.1f, 3.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoRuedaFIzq.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.45f, -1.1f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ejeRuedaFIzq.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.25f, -1.5f, 0.7f));
		model = glm::rotate(model, giroLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // la llanta gira sobre su eje
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaRuedaFIzq.RenderModel();

		// rueda frontal derecha
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-2.0f, 1.1f, -3.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseRuedaFDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.45f, -1.1f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ejeRuedaFDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.25f, -1.5f, -0.7f));
		model = glm::rotate(model, giroLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaRuedaFDer.RenderModel();

		// rueda central derecha
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.7f, 0.3f, -3.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoRuedaCDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.3f, -2.0f, -1.2f));
		model = glm::rotate(model, giroLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaRuedaCDer.RenderModel();

		// rueda trasera derecha
		model = modelaux2;
		model = glm::translate(model, glm::vec3(3.7f, 0.3f, -3.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoRuedaTDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.3f, -2.0f, -1.2f));
		model = glm::rotate(model, giroLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaRuedaCDer.RenderModel();

		// rueda central izquierda
		// esta rueda y la trasera izquierda estan volteadas, por eso su llanta gira al reves
		model = modelaux2;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-3.7f, 0.3f, -3.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoRuedaCDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.3f, -2.0f, -1.2f));
		model = glm::rotate(model, -giroLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaRuedaCDer.RenderModel();

		// rueda trasera izquierda
		model = modelaux2;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-2.7f, 0.3f, -3.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoRuedaTDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.3f, -2.0f, -1.2f));
		model = glm::rotate(model, -giroLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaRuedaCDer.RenderModel();

		// cuerpo del avion
		model = glm::mat4(1.0);
		model = glm::translate(model, posAvion);
		model = glm::rotate(model, anguloAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		modelaux2 = model;    // se guarda el cuerpo del avion para pegarle las turbinas
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Avion_M.RenderModel();

		// turbina izquierda, va debajo del ala
		model = modelaux2;
		model = glm::translate(model, glm::vec3(220.0f, -88.0f, 175.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Turbina_M.RenderModel();

		// helice izquierda, va al frente de su turbina y gira sobre su eje
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 79.0f));
		model = glm::rotate(model, giroHelice * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		// turbina derecha
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-220.0f, -88.0f, 175.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Turbina_M.RenderModel();

		// helice derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 79.0f));
		model = glm::rotate(model, giroHelice * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		// toda la pista se arma con el mismo pedazo
		// solo cambia donde va, su giro y su tamano
		pistaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// rampa de subida, se gira en Z para que suba hacia X positivo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(xInicioPista + largoRampa / 2.0f, alturaPista + alturaRampa / 2.0f, zRectaRampas));
		model = glm::rotate(model, inclinacionRampa * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(largoRampaInclinada, 1.0f, anchoPista));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();

		// recta de arriba, entre las dos rampas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, alturaPista + alturaRampa, zRectaRampas));
		model = glm::scale(model, glm::vec3(xFinPista - xInicioPista - 2.0f * largoRampa, 1.0f, anchoPista));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();

		// rampa de bajada, igual que la de subida pero girada al reves
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(xFinPista - largoRampa / 2.0f, alturaPista + alturaRampa / 2.0f, zRectaRampas));
		model = glm::rotate(model, -inclinacionRampa * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(largoRampaInclinada, 1.0f, anchoPista));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();

		// recta plana del otro lado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, alturaPista, zRectaPlana));
		model = glm::scale(model, glm::vec3(xFinPista - xInicioPista, 1.0f, anchoPista));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();

		// paredes de ladrillo debajo de las rampas y de la recta de arriba
		// va una de cada lado de la pista
		brickTexture.UseTexture();
		for (int lado = -1; lado <= 1; lado += 2)
		{
			float zPared = zRectaRampas + lado * anchoPista / 2.0f;

			// lado de la rampa de subida
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(xInicioPista + largoRampa / 2.0f, alturaPista, zPared));
			model = glm::scale(model, glm::vec3(largoRampa, alturaRampa, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();

			// lado de la rampa de bajada, es el mismo triangulo volteado
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(xFinPista - largoRampa / 2.0f, alturaPista, zPared));
			model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(largoRampa, alturaRampa, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();

			// lado de la recta de arriba, es el pedazo de pista parado
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, alturaPista + alturaRampa / 2.0f, zPared));
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(xFinPista - xInicioPista - 2.0f * largoRampa, 1.0f, alturaRampa));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[7]->RenderMeshModel();
		}
		pistaTexture.UseTexture();

		// las curvas se arman con pedazos chicos
		// puestos en media vuelta alrededor de su centro
		float centroCurvaZ = (zRectaRampas + zRectaPlana) / 2.0f;
		float largoPieza = radioCurva * 3.1416f / piezasCurva + 0.1f; // un poco mas largo para que no queden huecos
		for (int i = 0; i < piezasCurva; i++)
		{
			float anguloPieza = (i + 0.5f) * 180.0f / piezasCurva;

			// pedazo de la primera curva, del lado de X positivo
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(xFinPista + radioCurva * sin(anguloPieza * toRadians),
				alturaPista, centroCurvaZ + radioCurva * cos(anguloPieza * toRadians)));
			model = glm::rotate(model, anguloPieza * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(largoPieza, 1.0f, anchoPista));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[7]->RenderMeshModel();

			// pedazo de la segunda curva, del lado de X negativo
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(xInicioPista - radioCurva * sin(anguloPieza * toRadians),
				alturaPista, centroCurvaZ - radioCurva * cos(anguloPieza * toRadians)));
			model = glm::rotate(model, anguloPieza * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(largoPieza, 1.0f, anchoPista));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[7]->RenderMeshModel();
		}

		// la nave se inclina en X
		// para que su nariz suba o baje en las rampas
		model = glm::mat4(1.0);
		model = glm::translate(model, posNave);
		model = glm::rotate(model, anguloNave * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -inclinacionNave * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaNave, escalaNave, escalaNave));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		// cuerpo del satelite
		// se gira para que su frente vea hacia donde avanza
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(centroOrbita.x + radioOrbita * sin(anguloSatelite * toRadians),
			centroOrbita.y, centroOrbita.z + radioOrbita * cos(anguloSatelite * toRadians)));
		model = glm::rotate(model, (anguloSatelite + 90.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaSatelite, escalaSatelite, escalaSatelite));
		modelaux = model;    // se guarda el cuerpo para pegarle los paneles y la antena
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SateliteCuerpo_M.RenderModel();

		// panel derecho, gira sobre su brazo
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.6f, 0.0f, 0.0f));
		model = glm::rotate(model, giroPaneles * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatelitePanel_M.RenderModel();

		// panel izquierdo, es el mismo modelo volteado
		// por eso gira con el angulo negativo y los dos se inclinan igual
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.6f, 0.0f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -giroPaneles * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatelitePanel_M.RenderModel();

		// antena arriba del cuerpo, gira sobre su propio eje
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.74f, 0.3f));
		model = glm::rotate(model, giroAntena * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SateliteAntena_M.RenderModel();


		//Agave ¿qué sucede si lo renderizan antes del Rover?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMeshModel();
		
		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMeshModel();

		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		for (int i = 1; i<4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();

		 }

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();
		}
 

		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		*/
		toffsetnumerocambiau += 0.25; 
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		//cambiar automáticamente entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();
		
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
