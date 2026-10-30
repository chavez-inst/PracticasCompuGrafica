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

// variables del avion, que vuela haciendo un ocho
// el ocho se hace con senos y tiempoVuelo dice en que parte del ocho va el avion
glm::vec3 centroOcho = glm::vec3(0.0f, 16.0f, 8.6f);
float radioOcho = 15.0f;
// en una vuelta del ocho el avion sube y en la otra baja
float alturaOcho = 4.0f;
float tiempoVuelo = 0.5f;
float velocidadOcho = 0.005f;
glm::vec3 posAvion = glm::vec3(0.0f, 0.0f, 0.0f);
float anguloAvion = 0.0f;
float inclinacionAvion = 0.0f;
// cuando pasa por el centro del ocho, el avion da una vuelta completa sobre si mismo
float giroEje = 0.0f;
float duracionGiroEje = 0.6f;
float giroHelice = 0.0f;
float giroHeliceOffset = 20.0f;

// el humo son nubes que salen de atras de cada turbina
// de cada nube se guarda donde salio y cuando, para quitarla a los 2 segundos
struct Humo
{
	glm::vec3 posicion;
	float tiempoSalida;
};
std::vector<Humo> humo;
float duracionHumo = 2.0f;
// cada cuantos segundos sale una nube nueva
float intervaloHumo = 0.04f;
float tiempoUltimoHumo = 0.0f;
float tamanoHumo = 1.6f;

// variables del holocron
// sus 8 esquinas se separan del cuerpo, giran sobre si mismas y regresan
glm::vec3 posHolocron = glm::vec3(6.0f, 1.0f, -8.0f);
// estadoHolocron dice que esta haciendo: 0 esta quieto, 1 se separan las esquinas,
// 2 las esquinas giran y 3 regresan al cuerpo
int estadoHolocron = 0;
float esperaHolocron = 0.0f;
// cuanto tiempo se queda quieto antes de abrirse otra vez
float tiempoQuietoHolocron = 90.0f;
float separacionEsquinas = 0.0f;
float separacionMaxima = 1.0f;
float velocidadSeparacion = 0.01f;
float giroEsquinas = 0.0f;
float velocidadGiroEsquinas = 2.0f;

// variables del transbordador, que despega y se va curveando hasta quedar casi acostado
// su camino se calcula con ecuaciones a partir de tiempoCohete
glm::vec3 basePlataforma = glm::vec3(16.0f, -2.0f, -6.0f);
float tiempoCohete = 0.0f;
float velocidadTiempoCohete = 0.012f;
// la altura crece cada vez mas rapido, como si fuera acelerando
float aceleracionCohete = 0.5f;
// desde esta altura empieza a curvearse hacia X negativo
float alturaGiroCohete = 6.0f;
float curvaCohete = 0.015f;
// cuando llega a esta altura se regresa a la plataforma para despegar otra vez
float alturaMaximaCohete = 40.0f;
bool coheteEnPlataforma = true;
float esperaCohete = 0.0f;
float tiempoEnPlataforma = 120.0f;
glm::vec3 posCohete = glm::vec3(0.0f, 0.0f, 0.0f);
float inclinacionCohete = 0.0f;

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
Texture HumoTexture;



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

// holocron por piezas, el cuerpo y una esquina que se dibuja 8 veces
Model HolocronCuerpo_M;
Model HolocronEsquina_M;

// transbordador, sus cuatro piezas tienen el mismo origen y asi quedan armadas
Model Orbitador_M;
Model Tanque_M;
Model CoheteIzq_M;
Model CoheteDer_M;



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


// regresa donde va el transbordador
// primero solo sube, y cuando pasa alturaGiroCohete tambien avanza hacia X negativo
// cada vez mas, por eso su camino se va curveando
glm::vec3 posicionCohete(float t)
{
	glm::vec3 posicion = basePlataforma;
	float altura = aceleracionCohete * t * t;
	posicion.y += altura;
	if (altura > alturaGiroCohete)
	{
		float alturaDesdeGiro = altura - alturaGiroCohete;
		posicion.x -= curvaCohete * alturaDesdeGiro * alturaDesdeGiro;
	}
	return posicion;
}


// regresa en que punto del ocho va el avion
// en Z el seno va al doble de rapido que en X, y por eso se forma el ocho
glm::vec3 posicionOcho(float t)
{
	glm::vec3 posicion = centroOcho;
	posicion.x += radioOcho * sin(t);
	posicion.y += alturaOcho * sin(t);
	posicion.z += (radioOcho / 2.0f) * sin(2.0f * t);
	return posicion;
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

	// nube de humo, son tres planos cruzados para que se vea desde cualquier lado
	unsigned int humoIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4, 5, 6,
	   4, 6, 7,
	   8, 9, 10,
	   8, 10, 11
	};

	GLfloat humoVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,

		-0.5f, 0.0f, -0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, 0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
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

	MeshModel* obj8 = new MeshModel();
	obj8->CreateMeshModel(humoVertices, humoIndices, 96, 18);
	meshList.push_back(obj8); // nube de humo

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
	HumoTexture = Texture("Textures/humo.png");
	HumoTexture.LoadTextureA();

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

	HolocronCuerpo_M = Model();
	HolocronCuerpo_M.LoadModel("Models/Holocron/holocron_cuerpo.obj");
	HolocronEsquina_M = Model();
	HolocronEsquina_M.LoadModel("Models/Holocron/holocron_esquina.obj");

	Orbitador_M = Model();
	Orbitador_M.LoadModel("Models/Transbordador/transbordador_orbitador.obj");
	Tanque_M = Model();
	Tanque_M.LoadModel("Models/Transbordador/transbordador_tanque.obj");
	CoheteIzq_M = Model();
	CoheteIzq_M.LoadModel("Models/Transbordador/transbordador_cohete_izq.obj");
	CoheteDer_M = Model();
	CoheteDer_M.LoadModel("Models/Transbordador/transbordador_cohete_der.obj");
	

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

		// el avion avanza en el ocho
		tiempoVuelo += velocidadOcho * deltaTime;
		posAvion = posicionOcho(tiempoVuelo);

		// para saber hacia donde apunta la nariz
		// se ve donde va a estar el avion un poco despues
		glm::vec3 direccionAvion = posicionOcho(tiempoVuelo + 0.01f) - posAvion;
		anguloAvion = atan2(direccionAvion.x, direccionAvion.z) / toRadians;
		float avanceHorizontal = sqrt(direccionAvion.x * direccionAvion.x + direccionAvion.z * direccionAvion.z);
		inclinacionAvion = atan2(direccionAvion.y, avanceHorizontal) / toRadians;

		// el avion pasa por el centro del ocho cada vez que tiempoVuelo avanza 3.1416
		// tramoCentro dice que tan cerca va del centro y, si esta pasando por ahi, da la vuelta
		float tramoCentro = fmod(tiempoVuelo + duracionGiroEje / 2.0f, 3.1416f);
		if (tramoCentro < duracionGiroEje)
			giroEje = tramoCentro / duracionGiroEje * 360.0f;
		else
			giroEje = 0.0f;

		giroHelice += giroHeliceOffset * deltaTime;

		// animacion del holocron
		if (estadoHolocron == 0)
		{
			// el holocron se queda quieto un rato
			esperaHolocron += deltaTime;
			if (esperaHolocron > tiempoQuietoHolocron)
			{
				esperaHolocron = 0.0f;
				estadoHolocron = 1;
			}
		}
		else if (estadoHolocron == 1)
		{
			// las esquinas se alejan del cuerpo
			separacionEsquinas += velocidadSeparacion * deltaTime;
			if (separacionEsquinas >= separacionMaxima)
			{
				separacionEsquinas = separacionMaxima;
				estadoHolocron = 2;
			}
		}
		else if (estadoHolocron == 2)
		{
			// las esquinas dan una vuelta completa sobre si mismas
			giroEsquinas += velocidadGiroEsquinas * deltaTime;
			if (giroEsquinas >= 360.0f)
			{
				giroEsquinas = 0.0f;
				estadoHolocron = 3;
			}
		}
		else if (estadoHolocron == 3)
		{
			// las esquinas regresan al cuerpo
			separacionEsquinas -= velocidadSeparacion * deltaTime;
			if (separacionEsquinas <= 0.0f)
			{
				separacionEsquinas = 0.0f;
				estadoHolocron = 0;
			}
		}

		// animacion del transbordador
		if (coheteEnPlataforma)
		{
			// espera un rato en la plataforma antes de despegar
			esperaCohete += deltaTime;
			if (esperaCohete > tiempoEnPlataforma)
			{
				esperaCohete = 0.0f;
				tiempoCohete = 0.0f;
				coheteEnPlataforma = false;
			}
		}
		else
		{
			tiempoCohete += velocidadTiempoCohete * deltaTime;
			// si ya subio mucho, se regresa a la plataforma
			if (posicionCohete(tiempoCohete).y > basePlataforma.y + alturaMaximaCohete)
			{
				tiempoCohete = 0.0f;
				coheteEnPlataforma = true;
			}
		}
		posCohete = posicionCohete(tiempoCohete);
		// la punta se inclina hacia donde va el transbordador,
		// igual que con el avion se ve donde va a estar un poco despues
		glm::vec3 direccionCohete = posicionCohete(tiempoCohete + 0.01f) - posCohete;
		if (coheteEnPlataforma)
			inclinacionCohete = 0.0f;
		else
			inclinacionCohete = atan2(-direccionCohete.x, direccionCohete.y) / toRadians;

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
		// la nariz sube o baja segun si el avion va subiendo o bajando
		model = glm::rotate(model, -inclinacionAvion * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		// aqui el avion da la vuelta sobre si mismo
		model = glm::rotate(model, giroEje * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
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
		// el humo sale de la parte de atras de la turbina
		// con la matriz de la turbina se sabe donde queda ese punto en la escena
		glm::vec3 salidaHumoIzq = glm::vec3(model * glm::vec4(0.0f, 0.0f, -120.0f, 1.0f));

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
		glm::vec3 salidaHumoDer = glm::vec3(model * glm::vec4(0.0f, 0.0f, -120.0f, 1.0f));

		// helice derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 79.0f));
		model = glm::rotate(model, giroHelice * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		// cada cierto tiempo sale una nube nueva de cada turbina
		// y se queda donde salio aunque el avion siga avanzando
		if (now - tiempoUltimoHumo > intervaloHumo)
		{
			humo.push_back({ salidaHumoIzq, now });
			humo.push_back({ salidaHumoDer, now });
			tiempoUltimoHumo = now;
		}

		// se quitan las nubes que ya tienen mas de 2 segundos
		// las mas viejas son las primeras de la lista
		while (!humo.empty() && now - humo[0].tiempoSalida > duracionHumo)
			humo.erase(humo.begin());


		// cuerpo del holocron
		model = glm::mat4(1.0);
		model = glm::translate(model, posHolocron);
		modelaux = model;    // se guarda para colocar las esquinas a partir del cuerpo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		HolocronCuerpo_M.RenderModel();

		// esquinas del holocron
		// el modelo es la esquina de arriba que esta hacia X positivo y Z positivo
		// las 4 de arriba se sacan girandola de 90 en 90 grados en Y,
		// y las 4 de abajo ademas se voltean de cabeza
		glm::vec3 diagonalEsquina = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
		for (int i = 0; i < 8; i++)
		{
			model = modelaux;
			model = glm::rotate(model, (90.0f * (i % 4)) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			if (i >= 4)
				model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			// la esquina se pone en su lugar y se aleja del centro en diagonal
			model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.75f) + diagonalEsquina * separacionEsquinas);
			// gira sobre la linea que va de su punta al centro de su base
			model = glm::rotate(model, giroEsquinas * toRadians, diagonalEsquina);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			HolocronEsquina_M.RenderModel();
		}

		// transbordador, las cuatro piezas se dibujan con la misma matriz
		model = glm::mat4(1.0);
		model = glm::translate(model, posCohete);
		model = glm::rotate(model, inclinacionCohete * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Orbitador_M.RenderModel();
		Tanque_M.RenderModel();
		CoheteIzq_M.RenderModel();
		CoheteDer_M.RenderModel();

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

		// humo del avion
		// se dibuja al final y con glDepthMask apagado
		// para que la orilla transparente de una nube no tape a las de atras
		glDepthMask(GL_FALSE);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HumoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < humo.size(); i++)
		{
			// la nube se va haciendo mas chica hasta desaparecer a los 2 segundos
			float edadHumo = now - humo[i].tiempoSalida;
			float tamano = tamanoHumo * (1.0f - edadHumo / duracionHumo);
			model = glm::mat4(1.0);
			model = glm::translate(model, humo[i].posicion);
			model = glm::scale(model, glm::vec3(tamano, tamano, tamano));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[7]->RenderMeshModel();
		}
		glDepthMask(GL_TRUE);

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
