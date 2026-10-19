//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//Practica #9 											  Ruiz Vargas Ricardo
//Fecha de entrega: 17 de octubre de 2026 							316226068
//
// 

#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.5f, 2.2f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;


float movelightRot = 0.0f;


// Positions of the point lights		ARREGLO - Posiciones de las fuentes de luz
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.68f, 0.55f, 0.72f),		//Vela
	glm::vec3(3.12f, 1.71f, -2.86f),	//Pared
	glm::vec3(-2.88f, 1.05f, 2.0f),		//Piso
	glm::vec3(0.0f,30.0f, 50.0f)
};

float vertices[] = {
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

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



glm::vec3 Light1 = glm::vec3(0);


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 9 - Ricardo_Ruiz_Vargas", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	//Carga de shaders
	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	

	//Carga de modelos
	Model Dog((char*)"Models/RedDog.obj");
	Model Piso((char*)"Models/piso.obj");
	
	Model metroid1((char*)"Models/Metroid/metroid.obj");
	Model metroid2((char*)"Models/Metroid/metroid2.obj");
	Model metroid3((char*)"Models/Metroid/metroid3.obj");

	//Practica 9
	Model dog((char*)"Models/Perro/perro.obj");                    //Carga de ruta y nombre del objeto 
	Model lentes((char*)"Models/P6/LectureGlasses/lentes/lentes7-chick.fbx");
	Model silla((char*)"Models/P6/Silla/silla3.dae");
	Model mesa((char*)"Models/P6/Mesa/mesaZ.obj");
	Model cafe((char*)"Models/P6/Cafe/cafe.fbx");
	Model raton((char*)"Models/P6/LaptopMouse/raton/scene.gltf");
	Model compu((char*)"Models/P6/LaptopMouse/compu/laptop.glb");
	Model sillon((char*)"Models/P6/sillon/sofa.fbx");
	Model cuarto((char*)"Models/P6/habitacion/cuarto-2.obj");
	Model fondo((char*)"Models/P6/habitacion/fondo-cuarto.obj");

	Model luna((char*)"Models/P8/MajorasMask-Moon/Luna-MarioGalaxy.obj");
	Model sol((char*)"Models/P8/MarioGalaxy-Sun/Sol-MarioGalaxy2.obj");

	Model lamparapiso((char*)"Models/P9/Lamparas/Lampara-piso.fbx");
	Model lamparapared((char*)"Models/P9/Lamparas/Lampara-pared.fbx");
	Model lamparavela((char*)"Models/P9/Lamparas/vela.fbx");



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =													//Definición de los vertices del plano a manejar
	{
		// Positions            // Colors              // Texture Coords
		//// Frente
		//-0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,		0.31f,0.31f,
		// 0.5f, -0.5f, 0.5f,	   0.0f, 0.0f, 1.0f,		0.55f,0.31f,
		// 0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,	    0.55f,0.54f,
		//-0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,		0.31f,0.54f,

		//// Derecha
		// 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.31f,0.55f,
		// 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,		0.55f,0.55f,
		// 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,	    0.55f,0.80f,
		// 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.31f,0.80f,

		// // Atras
		//  0.5f, -0.5f, -0.5f,   0.0f, 0.0f,-1.0f,		0.56f,0.31f,
		// -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,-1.0f,		0.80f,0.31f,
		// -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,-1.0f,	    0.80f,0.54f,
		//  0.5f,  0.5f, -0.5f,   0.0f, 0.0f,-1.0f,		0.56f,0.54f,

		//  // Izquierda
		//  -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	    0.31f,0.05f,
		//  -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	    0.55f,0.05f,
		//  -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	    0.55f,0.30f,
		//  -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	    0.31f,0.30f,

		//  // Arriba
		//  -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		0.06f,0.31f,
		//   0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		0.30f,0.31f,
		//   0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,	    0.30f,0.54f,
		//  -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,		0.06f,0.54f,

		//  // Abajo
		//  -0.5f, -0.5f, -0.5f,   0.0f,-1.0f, 0.0f,	    0.68f,0.67f,
		//   0.5f, -0.5f, -0.5f,   0.0f,-1.0f, 0.0f,	    0.92f,0.67f,
		//   0.5f, -0.5f,  0.5f,   0.0f,-1.0f, 0.0f,	    0.92f,0.91f,
		//  -0.5f, -0.5f,  0.5f,   0.0f,-1.0f, 0.0f,	    0.68f,0.91f,

		// Frente (Z = 0.5 apunta hacia -Z)
		-0.5f, -0.5f, 0.5f,    0.0f, 0.0f, -1.0f,		0.31f,0.31f,
		 0.5f, -0.5f, 0.5f,	   0.0f, 0.0f, -1.0f,		0.55f,0.31f,
		 0.5f,  0.5f, 0.5f,    0.0f, 0.0f, -1.0f,	    0.55f,0.54f,
		-0.5f,  0.5f, 0.5f,    0.0f, 0.0f, -1.0f,		0.31f,0.54f,

		// Derecha (X = 0.5 apunta hacia -X)
		 0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,		0.31f,0.55f,
		 0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,		0.55f,0.55f,
		 0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	    0.55f,0.80f,
		 0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,		0.31f,0.80f,

		 // Atrás (Z = -0.5 apunta hacia +Z)
		  0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,		0.56f,0.31f,
		 -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,		0.80f,0.31f,
		 -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  1.0f,	    0.80f,0.54f,
		  0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  1.0f,		0.56f,0.54f,

		  // Izquierda (X = -0.5 apunta hacia +X)
		  -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,	    0.31f,0.05f,
		  -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,	    0.55f,0.05f,
		  -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,	    0.55f,0.30f,
		  -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,	    0.31f,0.30f,

		  // Arriba (Y = 0.5 apunta hacia -Y)
		  -0.5f,  0.5f,  0.5f,   0.0f, -1.0f, 0.0f,		0.06f,0.31f,
		   0.5f,  0.5f,  0.5f,   0.0f, -1.0f, 0.0f,		0.30f,0.31f,
		   0.5f,  0.5f, -0.5f,   0.0f, -1.0f, 0.0f,	    0.30f,0.54f,
		  -0.5f,  0.5f, -0.5f,   0.0f, -1.0f, 0.0f,		0.06f,0.54f,

		  // Abajo (Y = -0.5 apunta hacia +Y)
		  -0.5f, -0.5f, -0.5f,   0.0f,  1.0f, 0.0f,	    0.68f,0.67f,
		   0.5f, -0.5f, -0.5f,   0.0f,  1.0f, 0.0f,	    0.92f,0.67f,
		   0.5f, -0.5f,  0.5f,   0.0f,  1.0f, 0.0f,	    0.92f,0.91f,
		  -0.5f, -0.5f,  0.5f,   0.0f,  1.0f, 0.0f,	    0.68f,0.91f,

	};

	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,3,
		1,2,3,		//6

		4,5,6,
		6,7,4,		//12

		8,9,10,
		10,11,8,	//18

		12,13,14,
		14,15,12,	//24

		16,17,18,
		18,19,16,	//30

		20,21,22,
		22,23,20	//36

	};



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//Para el GLB se supone que se debe de cargar el archivo de textura de forma manual
	// Carga manual de la texxtura de la laptop:
	unsigned int compuTextura;
	glGenTextures(1, &compuTextura);
	glBindTexture(GL_TEXTURE_2D, compuTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int widthL, heightL, nrChannelsL;
	//stbi_set_flip_vertically_on_load(true);      // <--- VOLTEA LA IMAGEN
	unsigned char* dataL = stbi_load("Models/P6/LaptopMouse/compu/Image_0-inception-.jpg", &widthL, &heightL, &nrChannelsL, 0);
	//    unsigned char* dataL = stbi_load("Models/P6/LaptopMouse/compu/Image_0GG.jpg", &widthL, &heightL, &nrChannelsL, 0);    //Mi pantalla
	if (dataL) {
		GLenum format;
		if (nrChannelsL == 1) format = GL_RED;
		else if (nrChannelsL == 3) format = GL_RGB;
		else if (nrChannelsL == 4) format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, widthL, heightL, 0, format, GL_UNSIGNED_BYTE, dataL);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Fallo al cargar la textura de la compu" << std::endl;
	}
	stbi_image_free(dataL);
	// Inyección manual a todas las mallas del modelo
	for (unsigned int i = 0; i < compu.meshes.size(); i++) {
		compu.meshes[i].textures.clear();    // <--- ESTA LÍNEA ELIMINA LA TEXTURA NEGRA ROTA POR FIN!!        
		Texture texL;
		texL.id = compuTextura;
		texL.type = "texture_diffuse";
		texL.path = "Models/P6/LaptopMouse/compu/Image_0.jpg";
		compu.meshes[i].textures.push_back(texL);
	}
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	//---------------------------------------------------------------------------------------------------------------------------
	//Carga de la textura DADO --------------------------------------------------------------------------------------------------
	GLuint texture5;							//identificador de la textura
	glGenTextures(1, &texture5);				//enlace del identificador la textura con el tipo de elemento (textura)
	glBindTexture(GL_TEXTURE_2D, texture5);
	//int textureWidth, textureHeight, nrChannels;
	stbi_set_flip_vertically_on_load(true);		//volteo de la textura (para corregir el volteo de la libreria)
	//unsigned char* image;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// Diffuse map DADO - - - - - - - - - -
	//dataL = stbi_load("images/dado-colorido.jpg", &widthL, &heightL, &nrChannelsL, 0); // <-- Corregido a heightL
	dataL = stbi_load("images/Fondo1.jpg", &widthL, &heightL, &nrChannelsL, 0); // <-- Corregido a heightL
	if (dataL)
	{									
		glBindTexture(GL_TEXTURE_2D, texture5);
		// Detecta automáticamente si tu imagen JPG/PNG tiene transparencia o no
		GLenum format = (nrChannelsL == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, widthL, heightL, 0, format, GL_UNSIGNED_BYTE, dataL);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Fallo al cargar la textura del dado" << std::endl;
	}
	stbi_image_free(dataL);
	stbi_set_flip_vertically_on_load(false); // <--- 2. LO DESACTIVAMOS POR SEGURIDAD
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


	//// First, set the container's VAO (and VBO)
	/*GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);*/

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO; //Agregar EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO); //Generar el buffer de índices

	glBindVertexArray(VAO);

	// Configuración de Vértices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configuración de Índices (EBO)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // <-- 3. Enlazas el buffer EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // <-- 4. Le inyectas el arreglo 'indices'

	// Position attribute

	//// Position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//// normal attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);



	// Atributo de Posiciones (Salto de 8 floats)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo de Normales/Colores (Salto de 8 floats, inicia en el byte 3)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Atributo de Coordenadas de Textura UV (Salto de 8 floats, inicia en el byte 6)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);



	// Set texture units
	//manejo de las texturas
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);


	//glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 500.0f);


	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   
		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		
		
		//Load Model
	

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

                  glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// LUCES . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// Directional light . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . * * * * *
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);  //hacia donde apunta la luz
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.0f, 0.0f, 0.0f); 
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.02314f, 0.09f, 0.01608f);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"),0.0f, 0.0f, 0.0f);

		//	Para el sol y la luna		--> NO volver a experimentar, te va a quitar mucho tiempo ATTE: Ricardo del pasado [BORRRAR]
		//Se le calcula la rotacion de orbita usando la misma variable movelightRot como en la practica anterior
		glm::mat4 rotacion = glm::rotate(glm::mat4(1.0f), glm::radians(movelightRot), glm::vec3(0.0f, 0.0f, 1.0f));
			//intentar calcular la posición del Sol y la Luna aprovechando que estan en lados opuestos del cuarto
					//ajustar el '7.9f' si se necesita que la órbita sea más grande o más pequeña
			//se le define donde esta el centro de habitacion (o el punto sobre el que quierro que giren)
			glm::vec3 centroOrbita = glm::vec3(0.0f, 0.0f, -0.3f);		//ajustar la X o Z segun se necesite mover el aro de luz
			//Calcular la rotación (de radio de 7.9f en el eje X como en la pract anterior) y luego sumarle el centro
			glm::vec3 posicionSol = glm::vec3(rotacion * glm::vec4(7.9f, 0.0f, 0.0f, 1.0f)) + centroOrbita;
			glm::vec3 posicionLuna = glm::vec3(rotacion * glm::vec4(-7.9f, 0.0f, 0.0f, 1.0f)) + centroOrbita;
			//	--> Todo esto se hubiera ahorrado si lo hubiera hecho como en la anterior y solo modificarle poco

		//las variables temporales para la luz direccional
		glm::vec3 dirLightDirection, dirLightAmbient, dirLightDiffuse, dirLightSpecular;
		//Parte del Día y Noche (si la 'Y' del sol es positiva, está sobre el horizonte)
		if (posicionSol.y > -1.5f)								//-1.5 para que se vea el atardecer y amanecer apenas se ve el sol
		{
			//ES DE DÍA (El Sol se ilumina)
			dirLightDirection = -posicionSol;					//la luz apunta desde el sol hacia el centro
			dirLightAmbient = glm::vec3(0.15f, 0.15f, 0.15f);
			dirLightDiffuse = glm::vec3(0.9608f, 0.2863f, 0.1529f);		//Luz calida/amarillenta
			dirLightSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
		}
		else
		{
			//ES DE NOCHE (La Luna se ilumin)
			dirLightDirection = -posicionLuna;					//La luz apunta desde la luna hacia el centro
			dirLightAmbient = glm::vec3(0.015f, 0.015f, 0.015f);	
			dirLightDiffuse = glm::vec3(0.5529f, 0.7725f, 0.9412f);		//Luz algo tenue, fría y azulada
			dirLightSpecular = glm::vec3(0.2f, 0.2f, 0.3f);
		}
		//Ahora si se le envia los datos finales de la luz Direccional al shader con fe 
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), dirLightDirection.x, dirLightDirection.y, dirLightDirection.z);	//hacia donde apunta la luz
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), dirLightAmbient.x, dirLightAmbient.y, dirLightAmbient.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), dirLightDiffuse.x, dirLightDiffuse.y, dirLightDiffuse.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), dirLightSpecular.x, dirLightSpecular.y, dirLightSpecular.z);


		// Point light 1  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . * * * * *
	    glm::vec3 lightColor;
		lightColor.x= abs(sin(glfwGetTime() *Light1.x));
		lightColor.y= abs(sin(glfwGetTime() *Light1.y));			//sin(glfwGetTime())  se puede usar al parecer para simular 
		lightColor.z= sin(glfwGetTime() *Light1.z);					// el parpadeo de la luz, como de un fuego 

				//glfwGetTime() es el tiempo continuo (t) en segundos desde el inicio el programa
				
		float parpadeo = (sin(glfwGetTime() * 7.5f) + 1.0f) / 5.0f + 0.4f;
		//float parpadeo = (sin(glfwGetTime() * 8.0f) + 1.0f) / 5.0f + 0.6f;			
											// ^ 8.0
											// modifica la frecuencia de la onda ô.ó 
											// valor alto hace que el seno oscile muy rapido (titileo de vela), 
											// mientras que uno bajo hariaa un parpadeo +lento (luz de advertencia)

													// ^1.0	
											// la funcion del seno normal devuelve los valores entre -1.0 y 1.0 
											//al sumarle uno, el rango pasa a ser de 0.0 a 2.0 
											//Esto evita valores negativos que generan "luz oscura", restando luz a la escena
															// ^ 5.0
											//le comprime la amplitud de la onda 
											//El rango de 0.0 a 2.0 se divide ahora entre cinco, resultando en un nuevo rango 

											//de 0.0 a 0.4, asegurando que el cambio de brillo sea sutil y no apague la vela 
											//por completo

																	// ^ 0.6
											//Es el nivel base de iluminacion. Sumando esto al rango anterior, el valor 
											//final de la variable oscilaraa constantemente entre 0.6 y 1.0. La vela 
											//siempre estara encendida al menos a un 60% de su capacidad y llegara al 
											//100% en sus picos.

										//Idea para agregar al proyecto <----

												//Segun el indice de arreglo |/
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.949 * parpadeo, 0.6196f * parpadeo, 0.0863 * parpadeo);	// * parpadeo para alternar
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.9f * parpadeo, 0.9569f * parpadeo, 0.251f * parpadeo);	// la intensidad del valor
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.5f, 0.3f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);		//Factor
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 4.15f);		// de			alto = menos alcance
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"),0.85f);	//atenuacion	alto = se apaga +rapido

		// Point light 2  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.7f, 0.6f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.3f, 0.3f, 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.9f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.55f);

		// Point light 3  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.7f, 0.6f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.3f, 0.3f, 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.02f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.095);

		// Point light 4  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.15f, 0.08f, 0.04f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.75f, 0.45f, 0.15f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.4f, 0.3f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0075f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0085f);


		// SpotLight  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . * * * * *
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.8f, 0.8f, 0.8f);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 1.0f, 0.9843f);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"),0.0f, 0.0f, 0.0f);
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.2f);
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.8f);						//Angulo de apertura:
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(10.0f)));	 //interno
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(35.0f)));//externo

		//definir la posicion de la pantalla de la laptop
		glm::vec3 posLaptop = glm::vec3(-0.0f, 0.55f, 0.588f);									//punto origen
		//definimos hacia donde apunta (hacia la cara del perro)
		glm::vec3 direccionLaptop = glm::normalize(glm::vec3(0.0f, 0.65f, 0.0f) - posLaptop);	//punto destino
								//Se "normaliza" es como obtener el vector que apunte desde 
								// un Punto A hacia un Punto B, la formula es restar el destino 
								// menos el origen (B - A). 
								// Teniendo eso, es como si en el espacio se tuviera una flecha 
								// con inicio y fin para saber hacia donde se dirige la luz. 
								// El vector actua como un tipo de brujula para saber en que
								// angulo va a ir la luz
		// enviando los datos al shader
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), posLaptop.x, posLaptop.y, posLaptop.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), direccionLaptop.x, direccionLaptop.y, direccionLaptop.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2902f, 0.6706f, 0.9f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.2f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);				//Alcance real de la luz:
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.85f);					//alto = menos alcance
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.34f);				//alto = se apaga +rapido
																													//Angulo de apertura:
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(20.0f)));	 //interno
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(30.0f)));//externo



		// Set material properties
		// MATERIALES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);		//brillo

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1);

		

		//Carga de modelo / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
		// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        view = camera.GetViewMatrix();	
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Piso.Draw(lightingShader);						//piso en OBJ      <---
	
		//model = glm::mat4(1);
		////glEnable(GL_BLEND);					//***Activa la funcionalidad para trabajar el canal alfa
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);	//si tienen transparencia, cambiar el 0 por un 1
	 //   Dog.Draw(lightingShader);						//perro en OBJ      <---     
		////glDisable(GL_BLEND);				//***Desactiva el canal alfa 
		//glBindVertexArray(0);

		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(-0.2f, 0.2f, 0.0f));
		//glEnable(GL_BLEND);					//***Activa la funcionalidad para trabajar el canal alfa
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);	//si tienen transparencia, cambiar el 0 por un 1
		//metroid1.Draw(lightingShader);					//metroid en OBJ      <---     
		//glDisable(GL_BLEND);				//***Desactiva el canal alfa 
		//glBindVertexArray(0);

		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		//model = glm::rotate(model, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//glEnable(GL_BLEND);					//***Activa la funcionalidad para trabajar el canal alfa
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);	//si tienen transparencia, cambiar el 0 por un 1
		//metroid2.Draw(lightingShader);					//metroid en OBJ      <---     
		//glDisable(GL_BLEND);				//***Desactiva el canal alfa 
		//glBindVertexArray(0);

		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(1.0f, 0.2f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//glEnable(GL_BLEND);					//***Activa la funcionalidad para trabajar el canal alfa
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);	//si tienen transparencia, cambiar el 0 por un 1
		//metroid3.Draw(lightingShader);					//metroid en OBJ      <---     
		//glDisable(GL_BLEND);				//***Desactiva el canal alfa 
		//glBindVertexArray(0);

		//PRACTICA / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ 
		//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(8.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		dog.Draw(lightingShader);                       //perro en OBJ      <---     

		//model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, glm::vec3(0.0f, 0.62f, -0.22f));
		model = glm::rotate(model, glm::radians(-82.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(1.3f, 1.0f, 1.0f)); 
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glEnable(GL_BLEND);                                     //Agregar la
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      //transparencia
		lentes.Draw(lightingShader);                    //lentes en FBX     <---
		//glDisable(GL_BLEND);

		model = glm::translate(model, glm::vec3(0.0f, -0.3f, -1.12f));
		model = glm::rotate(model, glm::radians(-8.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(1.3f, 1.0f, 1.0f)); 
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);                     //silla en DAE      <---

		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 1.15f, 1.2f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);                      //mesa en OBJ       <---

		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.01f, -0.022f, -0.006f));
		//model = glm::scale(model, glm::vec3(1.3f, 1.15f, 1.2f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		cafe.Draw(lightingShader);                      //cafe en FBX       <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, glm::vec3(-0.32f, 0.38f, 0.3f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(15.3f, 15.15f, 15.2f));
		model = glm::scale(model, glm::vec3(0.009f, 0.009f, 0.009f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		raton.Draw(lightingShader);                     //raton en GLtF     <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, glm::vec3(-0.0f, 0.388f, 0.55f));
		model = glm::scale(model, glm::vec3(0.05f, 0.07f, 0.07f));
		model = glm::rotate(model, glm::radians(91.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		compu.Draw(lightingShader);                     //compu en GLB      <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		sillon.Draw(lightingShader);                    //sofa en FBX       <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-4.0f, 0.25f, 0.3f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glEnable(GL_BLEND);                                     //Agregar la
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      //transparencia
		glBindVertexArray(VAO);
		cuarto.Draw(lightingShader);                    //cuarto en OBJ     <---
		//glDisable(GL_BLEND);

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-4.0f, 0.25f, 0.3f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		fondo.Draw(lightingShader);						//fondo en OBJ     <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, glm::vec3(-0.8f, -0.52f, 0.1f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		lamparapiso.Draw(lightingShader);				//lampara en FBX     <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, glm::vec3(1.0f, -0.2f, -4.47f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		lamparapared.Draw(lightingShader);				//lampara en FBX     <---

		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.41f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		lamparavela.Draw(lightingShader);				//vela en FBX     <---

				//model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
				//model = glm::translate(model, glm::vec3(0.68f, 0.55f, 0.72f));
				//model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
				//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				//luna.Draw(lightingShader);		//VELA

				//model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
				//model = glm::translate(model, glm::vec3(3.12f, 1.71f, -2.86f));
				//model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
				//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				//luna.Draw(lightingShader);		//PARED

				//model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
				//model = glm::translate(model, glm::vec3(-2.88f, 1.05f, 2.0f));
				//model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
				//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				//luna.Draw(lightingShader);		//PISO


				model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
				model = glm::translate(model, glm::vec3(0.0f, 30.0f, 50.0f));
				model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
				glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				sol.Draw(lightingShader);		//PISO


		

		// Practica - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture5);

		// Inyectamos la textura a TODOS los posibles nombres que tu shader pueda estar usando internamente
		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "texture_diffuse1"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);

		// <-- NUEVO: Prevenimos que intente usar el mapa de reflejos de la laptop o silla
		glUniform1i(glGetUniformLocation(lightingShader.Program, "texture_specular1"), 0);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f); // Le subimos el brillo al dad

		// Lo subimos sobre la mesa (Y=0.45) y lo escalamos al 15% de su tamaño
		glm::vec3 posicionDado = glm::vec3(-0.3f, 0.45f, 0.4f);
		glm::vec3 escalaDado = glm::vec3(100.0f);

		glBindVertexArray(VAO);
		// 
		//Lado 1 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - FRENTE
		model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionDado);
		model = glm::scale(model, escalaDado);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
		////model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		////
		//glBindTexture(GL_TEXTURE_2D, texture5);
		////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		//Lado 2 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - DERECHA
		model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionDado);
		model = glm::scale(model, escalaDado);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
		////model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		////model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		////
		//glBindTexture(GL_TEXTURE_2D, texture5);
		////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));/////////////////
		//glBindVertexArray(0);

		//Lado 3 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ATRAS
		model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionDado);
		model = glm::scale(model, escalaDado);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));
		////model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		////model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		////
		//glBindTexture(GL_TEXTURE_2D, texture5);
		////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));
		//glBindVertexArray(0);

		//Lado 4 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - IZQUIERDA
		model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionDado);
		model = glm::scale(model, escalaDado);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));
		////model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
		////model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		////
		//glBindTexture(GL_TEXTURE_2D, texture5);
		////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));
		//glBindVertexArray(0);

		//Lado 5 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ARRIBA
		model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionDado);
		model = glm::scale(model, escalaDado);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
		////model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		////model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		////
		//glBindTexture(GL_TEXTURE_2D, texture5);
		////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
		//glBindVertexArray(0);

		//Lado 6 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ABAJO
		model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionDado);
		model = glm::scale(model, escalaDado);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
		glBindVertexArray(0);
		////model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		////model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		////
		//glBindTexture(GL_TEXTURE_2D, texture5);
		////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
		//glBindVertexArray(0);







		// FUENTES DE LUZ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
		// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();

		//// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		//modelLoc = glGetUniformLocation(lampShader.Program, "model");
		//viewLoc = glGetUniformLocation(lampShader.Program, "view");
		//projLoc = glGetUniformLocation(lampShader.Program, "projection");

		//// Set matrices
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		//model = glm::mat4(1);
		//model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//// Draw the light object (using light's vertex attributes)
		//for (GLuint i = 0; i < 4; i++)
		//{
		//	model = glm::mat4(1);
		//	model = glm::translate(model, pointLightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	glBindVertexArray(VAO);
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}
		//glBindVertexArray(0);

		// LUNA = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionLuna);
			//model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, glm::radians(movelightRot), glm::vec3(0.0f, 0.0f, 1.0f));        //Punto de giro
			////model = glm::translate(model, glm::vec3(7.0f, 0.0f, 0.0f));                                 //Posision donde estará inicialmente
			//model = glm::translate(model, lightPos);                     //Posision donde estará inicialmente
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		luna.Draw(lampShader);                          //luna en OBJ     <---

		// SOL  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
		model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
		model = glm::translate(model, posicionSol); 
			//model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, glm::radians(movelightRot), glm::vec3(0.0f, 0.0f, 1.0f));        //Punto de giro
			////model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));                                //Posision donde estará inicialmente
			//model = glm::translate(model, lightPos2);                                //Posision donde estará inicialmente
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		sol.Draw(lampShader);                           //sol en OBJ     <---



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);


	}

	//if (keys[GLFW_KEY_T])		//Derecha
	//{
	//	pointLightPositions[0].x += 0.01f;
	//}
	//if (keys[GLFW_KEY_G])		//Izquierda
	//{
	//	pointLightPositions[0].x -= 0.01f;
	//}

	//if (keys[GLFW_KEY_Y])		//Arriba
	//{
	//	pointLightPositions[0].y += 0.01f;
	//}

	//if (keys[GLFW_KEY_H])		//Abajo
	//{
	//	pointLightPositions[0].y -= 0.01f;
	//}
	//if (keys[GLFW_KEY_U])		//Adelante
	//{
	//	pointLightPositions[0].z -= 0.01f;
	//}
	//if (keys[GLFW_KEY_J])		//Atras
	//{
	//	pointLightPositions[0].z += 0.01f;
	//}
	
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}

	//Movimiento para la rotacion Sol y Luna _________________________________
	if (keys[GLFW_KEY_Q])       //ANTIHORARIO
	{
		movelightRot += 5.0f;
	}
	if (keys[GLFW_KEY_E])       //HORARIO
	{
		movelightRot -= 5.0f;
	}

}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}