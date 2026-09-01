#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>

// Se añade la lib <windows.h> para el sleep
#include <windows.h>

// Tambien las libs para rand, srand y time

#include <windows.h>
#include <stdlib.h>
#include <time.h>

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
color = vec4(1.0f, 1.0f, 1.0f, 1.0f); 			\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,0.0f,0.0f,1.0f);	 			\n\
}";



void CrearIniciales()
{
	// 3 letras (CLA) hechas de rectangulos rectos 
	// (2 triangulos por cada una = 54 vertices)
	GLfloat vertices[] = {
		//LETRA C
		-0.8f, -0.5f, 0.0f,  -0.7f, -0.5f, 0.0f,  -0.8f,  0.5f, 0.0f,
		-0.7f, -0.5f, 0.0f,  -0.7f, 0.5f, 0.0f,  -0.8f,  0.5f, 0.0f,
		-0.7f, 0.4f, 0.0f,  -0.4f, 0.4f, 0.0f,  -0.7f,  0.5f, 0.0f,
		-0.4f, 0.4f, 0.0f,  -0.4f, 0.5f, 0.0f,  -0.7f,  0.5f, 0.0f,
		-0.7f, -0.5f, 0.0f,  -0.4f, -0.5f, 0.0f,  -0.7f, -0.4f, 0.0f,
		-0.4f, -0.5f, 0.0f,  -0.4f, -0.4f, 0.0f,  -0.7f, -0.4f, 0.0f,

		//LETRA L
		-0.2f, -0.5f, 0.0f,  -0.1f, -0.5f, 0.0f,  -0.2f,  0.5f, 0.0f,
		-0.1f, -0.5f, 0.0f,  -0.1f,  0.5f, 0.0f,  -0.2f,  0.5f, 0.0f,
		-0.1f, -0.5f, 0.0f,  0.2f, -0.5f, 0.0f,  -0.1f, -0.4f, 0.0f,
		 0.2f, -0.5f, 0.0f,  0.2f, -0.4f, 0.0f,  -0.1f, -0.4f, 0.0f,

		 //LETRA A
		   0.4f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,   0.4f,  0.5f, 0.0f,
		   0.5f, -0.5f, 0.0f,  0.5f,  0.5f, 0.0f,   0.4f,  0.5f, 0.0f,
		   0.7f, -0.5f, 0.0f,  0.8f, -0.5f, 0.0f,   0.7f,  0.5f, 0.0f,
		   0.8f, -0.5f, 0.0f,  0.8f,  0.5f, 0.0f,   0.7f,  0.5f, 0.0f,
		   0.5f, 0.4f, 0.0f,  0.7f,  0.4f, 0.0f,   0.5f, 0.5f, 0.0f,
		   0.7f, 0.4f, 0.0f,  0.7f,  0.5f, 0.0f,   0.5f, 0.5f, 0.0f,
		   0.5f, 0.0f, 0.0f,  0.7f,  0.0f, 0.0f,   0.5f, 0.1f, 0.0f,
		   0.7f, 0.0f, 0.0f,  0.7f,  0.1f, 0.0f,   0.5f, 0.1f, 0.0f
	};

	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gráfica
	//verificaciones y prevención de errores
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
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Iniciales CLA", NULL, NULL);
	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	// Se inicia un num aleatorio para que cambie en cada ejecucion
	srand((unsigned int)time(NULL));

	//Llamada a las funciones creadas antes del main
	CrearIniciales();
	CompileShaders();

	// Se declaran los colores dentro de un arreglo
	GLfloat colores[3][3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	// Se inicializa un contador para que pueda ir cambiando de 
	// color en cada iteración del while
	int indiceColor = 0;


	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		// Generar color RGB aleatorio en cada ciclo
		float r = (float)rand() / RAND_MAX;
		float g = (float)rand() / RAND_MAX;
		float b = (float)rand() / RAND_MAX;

		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);

		// Ahora se dibujan 12 vertices en lugar de los 3 originales
		glDrawArrays(GL_TRIANGLES, 0, 54);
		glBindVertexArray(0);

		glUseProgram(0);

		// Se cambia el contador de los colores al siguiente en cada iteración
		indiceColor = (indiceColor + 1) % 3;
		// Se usa sleep para que el cambio de color sea perceptible 
		// cambiado a 2000 para los 2segs
		Sleep(2000);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA LÍNEA DESPUÉS DE glfwSwapBuffers(mainWindow); 
	}


	return 0;
}