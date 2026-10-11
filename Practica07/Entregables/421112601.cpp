/*
Práctica 7: Iluminación 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <string>
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

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture AgaveTexture;


Model Blackhawk_M;
//Sustituir por sus propios Rovers Con jerarquía
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
Model llantaRuedaFDer;
Model brazoRuedaCDer;
Model brazoRuedaTDer;
Model llantaRuedaCDer;
//Añadir modelo del avión.
Model Avion_M;


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

// posicion del rover y hacia donde avanza (el frente del modelo apunta a -x)
glm::vec3 posRover = glm::vec3(1.0f, -0.4f, 6.0f);
glm::vec3 dirRover = glm::vec3(-1.0f, 0.0f, 0.0f);
// el rover se hizo grande, asi que se reduce al dibujarlo
float escalaRover = 0.2f;

// posicion del avion y hacia donde avanza (la nariz del modelo apunta a +z)
glm::vec3 posAvion = glm::vec3(-6.0f, 6.0f, 0.0f);
glm::vec3 dirAvion = glm::vec3(0.0f, 0.0f, 1.0f);

// que tan lejos del centro del modelo se coloca cada faro
glm::vec3 offsetFaroRover = dirRover * 1.3f + glm::vec3(0.0f, 0.4f, 0.0f);  // al frente 
glm::vec3 offsetFaroAvionFrente = dirAvion * 4.0f;   // en la nariz
glm::vec3 offsetFaroAvionAbajo = glm::vec3(0.0f, -1.2f, 0.0f);  // debajo 

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
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
        //    x      y      z            u     v            nx     ny    nz
            -1.0f, -1.0f, -0.6f,    0.0f, 0.0f,        0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 1.0f,        0.5f, 0.0f,        0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -0.6f,        1.0f, 0.0f,        0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,        0.5f, 1.0f,        0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f,    0.0f, 0.0f,        0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,    10.0f, 0.0f,    0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,    0.0f, 10.0f,    0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,        10.0f, 10.0f,    0.0f, -1.0f, 0.0f
    };

    unsigned int vegetacionIndices[] = {
       0, 1, 2,
       0, 2, 3,
       4,5,6,
       4,6,7
    };

    GLfloat vegetacionVertices[] = {
        -0.5f, -0.5f, 0.0f,        0.0f, 0.0f,        0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,        1.0f, 0.0f,        0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,        1.0f, 1.0f,        0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,        0.0f, 1.0f,        0.0f, 0.0f, 0.0f,

        0.0f, -0.5f, -0.5f,        0.0f, 0.0f,        0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.5f,        1.0f, 0.0f,        0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f,        1.0f, 1.0f,        0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, -0.5f,        0.0f, 1.0f,        0.0f, 0.0f, 0.0f,


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

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);

    calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}


// para mover el rover y el avion con el teclado (w a s d se quedan para la camara)
// rover: flecha arriba avanza, flecha abajo retrocede
// avion: i avanza, k retrocede, o sube, l baja
void MoverVehiculos(bool* keys, GLfloat deltaTime)
{
    GLfloat velocidad = 0.1f * deltaTime;

    if (keys[GLFW_KEY_UP])     posRover += dirRover * velocidad;
    if (keys[GLFW_KEY_DOWN]) posRover -= dirRover * velocidad;

    if (keys[GLFW_KEY_I]) posAvion += dirAvion * velocidad;
    if (keys[GLFW_KEY_K]) posAvion -= dirAvion * velocidad;
    if (keys[GLFW_KEY_O]) posAvion.y += velocidad;
    if (keys[GLFW_KEY_L]) posAvion.y -= velocidad;

    // tope para que las ruedas del avion no atraviesen el piso
    if (posAvion.y < 0.8f) posAvion.y = 0.8f;
}


int main()
{
    mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
    mainWindow.Initialise();
    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

    pisoTexture = Texture("Textures/piso.tga");
    pisoTexture.LoadTextureA();
    AgaveTexture = Texture("Textures/Agave.tga");
    AgaveTexture.LoadTextureA();

    Blackhawk_M = Model();
    Blackhawk_M.LoadModel("Models/uh60.obj");

    // rover con jerarquia 
    std::string carpetaRover = "Models/RoverConJerarquia/";
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
    llantaRuedaFDer = Model();
    llantaRuedaFDer.LoadModel(carpetaRover + "rover_llanta_delantera_der.obj");
    brazoRuedaCDer = Model();
    brazoRuedaCDer.LoadModel(carpetaRover + "rover_suspension_central_der.obj");
    brazoRuedaTDer = Model();
    brazoRuedaTDer.LoadModel(carpetaRover + "rover_suspension_trasera_der.obj");
    llantaRuedaCDer = Model();
    llantaRuedaCDer.LoadModel(carpetaRover + "rover_llanta_central_der.obj");

    //Añadir modelo del avión
    // avion de la practica 6 (toma la textura de su .mtl)
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

    Material_brillante = Material(4.0f, 256);
    Material_opaco = Material(0.15f, 4);


    //luz direccional, sólo 1 y siempre debe de existir
    // luz tenue para que se noten mas los faros
    mainLight = DirectionalLight(0.85f, 0.9f, 1.0f,
        0.15f, 0.1f,
        1.0f, -1.0f, -1.0f);
    //contador de luces puntuales
    unsigned int pointLightCount = 0;
    //Declaración de primer luz puntual
    pointLights[0] = PointLight(0.85f, 0.9f, 1.0f,
        0.0f, 0.2f,
        -2.0f, 10.0f, 10.0f,
        1.0f, 0.05f, 0.005f);
    pointLightCount++;

    pointLights[1] = PointLight(0.85f, 0.9f, 1.0f,
        0.0f, 0.2f,
        -10.0f, 2.0f, 10.0f,
        1.0f, 0.05f, 0.005f);
    pointLightCount++;

    pointLights[2] = PointLight(0.85f, 0.9f, 1.0f,
        0.0f, 0.2f,
        -2.0f, 2.0f, 2.0f,
        1.0f, 0.05f, 0.005f);
    pointLightCount++;

    // el shader acepta maximo 4 spotlights (linterna, rover y dos del avion)
    unsigned int spotLightCount = 0;
    //linterna
    // un poco mas suave para que no tape a los faros
    spotLights[0] = SpotLight(1.0f, 0.8f, 0.45f,
        0.0f, 0.5f,
        0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        10.0f);
    spotLightCount++;

    // faro del rover (apunta adelante y un poco abajo para que se vea en el piso)
    glm::vec3 dirFaroRover = dirRover + glm::vec3(0.0f, -0.3f, 0.0f);
    spotLights[1] = SpotLight(0.9f, 0.95f, 1.0f,
        0.0f, 3.0f,
        posRover.x, posRover.y, posRover.z,
        dirFaroRover.x, dirFaroRover.y, dirFaroRover.z,
        1.0f, 0.02f, 0.002f,
        25.0f);
    spotLightCount++;

    // faro de abajo del avion (color azul para que se distinga)
    spotLights[2] = SpotLight(0.3f, 0.6f, 1.0f,
        0.0f, 4.0f,
        posAvion.x, posAvion.y, posAvion.z,
        0.0f, -1.0f, 0.0f,
        1.0f, 0.02f, 0.002f,
        25.0f);
    spotLightCount++;

    // faro de enfrente del avion (inclinado hacia abajo para que se alcance a ver en el piso)
    glm::vec3 dirFaroAvion = dirAvion + glm::vec3(0.0f, -0.5f, 0.0f);
    spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
        0.0f, 4.0f,
        posAvion.x, posAvion.y, posAvion.z,
        dirFaroAvion.x, dirFaroAvion.y, dirFaroAvion.z,
        1.0f, 0.02f, 0.002f,
        20.0f);
    spotLightCount++;
    //se crean mas luces puntuales y spotlight 

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0;
    GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
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
        MoverVehiculos(mainWindow.getsKeys(), deltaTime);

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

        //información en el shader de intensidad especular y brillo
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

        // luz ligada a la cámara de tipo flash
        //sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

        // actualizamos la posicion de los faros cada frame para que sigan al rover y al avion
        spotLights[1].SetPos(posRover + offsetFaroRover);
        spotLights[2].SetPos(posAvion + offsetFaroAvionAbajo);
        spotLights[3].SetPos(posAvion + offsetFaroAvionFrente);


        //información al shader de fuentes de iluminación
        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);



        glm::mat4 model(1.0);
        glm::mat4 modelaux(1.0);
        glm::mat4 modelaux2(1.0);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));

        pisoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshListModel[2]->RenderMeshModel();

        //BLACKHAWK
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Blackhawk_M.RenderModel();

        //Rover: Sustituir por su propio Rover con jerarquía
        // cuerpo principal del rover (de aqui cuelgan todas las demas piezas)
        model = glm::mat4(1.0);
        model = glm::translate(model, posRover);
        model = glm::scale(model, glm::vec3(escalaRover, escalaRover, escalaRover));
        modelaux2 = model;    // guarda el cuerpo para colgar de el cada rueda y parte
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        roverCuerpo.RenderModel();

        // brazo del rover (cada pieza cuelga de la anterior)
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
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        llantaRuedaFDer.RenderModel();

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
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        llantaRuedaCDer.RenderModel();

        // rueda central izquierda (se gira 180 para usar las piezas del lado derecho)
        model = modelaux2;
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(-3.7f, 0.3f, -3.2f));
        modelaux = model;
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        brazoRuedaCDer.RenderModel();

        model = modelaux;
        model = glm::translate(model, glm::vec3(-2.3f, -2.0f, -1.2f));
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
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        llantaRuedaCDer.RenderModel();


        //Añadir modelo del avión
        // igual que el rover, se usa posAvion para que sus faros lo sigan
        model = glm::mat4(1.0);
        model = glm::translate(model, posAvion);
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        Avion_M.RenderModel();



        //Agave ¿qué sucede si lo renderizan antes del helicóptero?
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        //blending: transparencia o traslucidez
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        AgaveTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshListModel[3]->RenderMeshModel();
        glDisable(GL_BLEND);

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}