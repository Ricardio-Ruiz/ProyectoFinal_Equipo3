//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//Practica #8 											  Ruiz Vargas Ricardo
//Fecha de entrega: 10 de octubre de 2026 							316226068
//
// 

// Std. Includes
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"
// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();


// Camera
Camera camera(glm::vec3(0.0f, 0.5f, 2.2f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;


// Light attributes
//glm::vec3 lightPos(0.5f, 0.5f, 2.5f);           //posición de la luz
glm::vec3 lightPos(7.9f, 0.0f, -0.3f);           //PRACTICA - - - - -
float movelightPos = 0.0f;                      //variable para la manipulación de la luz
GLfloat deltaTime = 0.0f;                   //Elementos para poder intercambiarlos
GLfloat lastFrame = 0.0f;                   // durante cada frame segun la rotación
float rot = 0.0f;
bool activanim = false;

    //Previo 8 - - - - - - - - - - - - - - - - - 
//glm::vec3 lightPos2(-0.64f, 0.7f, 3.5f);          //Nueva fuente de luz
glm::vec3 lightPos2(-7.9f, 0.0f, -0.3f);          //PRACTICA - - - - -
float movelightPos2x = 0.0f;                      //variables para la manipulación de la luz 2
float movelightPos2y = 0.0f;
float movelightPos2z = 0.0f;

float movelightRot = 0.0f;



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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 8 - Ricardo_Ruiz_Vargas", nullptr, nullptr);

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
    //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

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

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader shader("Shader/modelLoading.vs", "Shader/modelLoading.frag");
    Shader lampshader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");



    // Load models
    Model red_dog((char*)"Models/Perro/RedDog.obj");
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    //Previo 8
    Model zinnia_pose((char*)"Models/Zinnia/zinnia-pose.obj");
    //Model majora((char*)"Models/Majora/majora.obj");

    //Practica 6
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


    //// Build and compile our shader program
    //Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");				//Los archivos que se agregaron

    float vertices[] = {
        //Components x,y,z      Vector normal
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   //Ahora representan hacia donde va a estar apuntando el vector normal 
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   //Esto para ayudar a generar la simulación según se genere el reflejo
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   // que genera la fuente de luz y el vector nomal
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

        //// Positions            // Colors              // Texture Coords
        //- 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,1.0f,	0.0f,0.0f,
        //0.5f, -0.5f, 0.0f,	   1.0f, 1.0f,1.0f,		1.0f,0.0f,
        //0.5f,  0.5f, 0.0f,     1.0f, 1.0f,1.0f,	    1.0f,1.0f,
        //-0.5f,  0.5f, 0.0f,    1.0f, 1.0f,1.0f,		0.0f,1.0f,
    };
    //GLuint indices[] =
    //{  // Note that we start from 0!
    //    36,37,38,
    //    38,39,26,		//6
    //};




    // First, set the container's VAO (and VBO)
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load textures ------------------------------------------------------------------------------------------------------------
    // Textura perro ----------------------------------------------------------------------------------------
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    image = stbi_load("Models/Perro/Texture_albedo.jpg", &textureWidth, &textureHeight, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(image);
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    //--------------------------------------------------------------------------------------------------------
    //Carga de la textura2 -----------------------------------------------------------------------------------
    GLuint texture2;							//identificador de la textura
    glGenTextures(1, &texture2);				//enlace del identificador la textura con el tipo de elemento (textura)
    glBindTexture(GL_TEXTURE_2D, texture2);
    //int textureWidth, textureHeight, nrChannels;
    //stbi_set_flip_vertically_on_load(true);		//volteo de la textura (para corregir el volteo de la libreria)
    //unsigned char* image;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // Diffuse map - - - - - - - - - -
    image = stbi_load("Models/P6/habitacion/plant1.png", &textureWidth, &textureHeight, &nrChannels, 0);		//Carga de la ruta de la texturA
    glBindTexture(GL_TEXTURE_2D, texture2);		// se vincula ahora si la imagen con el identificador
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);	//RGBA para transparecnia
    glGenerateMipmap(GL_TEXTURE_2D);			//optimizar recursos segun la distancia
    if (image)
    {									//RGBA para transparecnia					  |/
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(image);
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Textura cielo ----------------------------------------------------------------------------------------
    GLuint texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    image = stbi_load("Models/P6/habitacion/Mario64DS-Background-Sky.png", &textureWidth, &textureHeight, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(image);
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Matriz que ayuda a controlar la posicion de la luna y sol
        glm::mat4 modelTemp = glm::mat4(1.0f); //Temp
            //no la usé ja

        // LUCES . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
        //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
        lightingShader.Use();
        
        //Parte necesaria para poder muver las luces junto con las esfersas
        glm::mat4 rotacion = glm::rotate(glm::mat4(1.0f), glm::radians(movelightRot), glm::vec3(0.0f, 0.0f, 1.0f)); //rotacion
        //posiciones ya rotadas:                                         ^sumar al teclear Q o E
        glm::vec3 luz1Rotada = glm::vec3(rotacion * glm::vec4(lightPos, 1.0f));
        glm::vec3 luz2Rotada = glm::vec3(rotacion * glm::vec4(lightPos2, 1.0f));
        //                                                      
        
        // Primera luz . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
        GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position"); //se carga el shader en la posicion
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");         // y vista
            //glUniform3f(lightPosLoc, lightPos.x + movelightPos, lightPos.y + movelightPos, lightPos.z + movelightPos);
                                            //^suma la variable para poder cambiar la posicion para cada eje por igual
        glUniform3f(lightPosLoc, luz1Rotada.x, luz1Rotada.y, luz1Rotada.z);                 
                //^^^nueva posicion durante la rotación^^^
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        // Set lights properties - - - - - - - - - - - - - - - - - - - - - - - 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);       //Para cada 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 0.5529f, 0.7725f, 0.9412f);    //una de las
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 0.2f, 0.2f, 0.4f);      //componentes


        //Config Segunda Luz (parámetros diferentes) ? . . . . . . . . . . . . . . . . . . . . . . .
        GLint lightPosLoc2 = glGetUniformLocation(lightingShader.Program, "light2.position");
        //GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");         // y vista ya no hace falta
            //glUniform3f(lightPosLoc2, lightPos2.x + movelightPos2x, lightPos2.y + movelightPos2y, lightPos2.z + movelightPos2z);
                // Quise hacer que la luz 2 se moviera con mas libertad
        glUniform3f(lightPosLoc2, luz2Rotada.x, luz2Rotada.y, luz2Rotada.z);                //nueva posicion durante la rotación
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.ambient"), 0.15f, 0.15f, 0.15f);       //Que tan iluminado es
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.diffuse"), 0.9608f, 0.2863f, 0.1529f);       //""Color""
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light2.specular"), 0.1f, 0.1f, 0.1f);         //Reflejo


        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        
        // MATERIALES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Set material properties - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 0.4f, 0.5f, 0.55f);   //Para cada 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0.8f, 0.9f, 0.95f);   //una de las    +es brillo  -es mas oscuro
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 0.5f, 0.6f, 0.5f);  //componentes
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 0.7f);      //--> Para el brillo


        // MODELOS / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        // / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        // Draw the loaded model / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        //glm::mat4 model(1);
        //model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        //glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //glBindVertexArray(VAO);                 //^Al objeto se le manda el shader
        ////glDrawArrays(GL_TRIANGLES, 0, 36);        //caja
        //red_dog.Draw(lightingShader);               //perro
        //glBindVertexArray(0);
        //
        //model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
        ////////model = glm::rotate(model, glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::translate(model, glm::vec3(-1.3f, -1.12f, 0.0f)); 
        ////model = glm::scale(model, glm::vec3(3.0f, 2.0f, 2.5f));
        ////model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //zinnia_pose.Draw(lightingShader);                   //Zinnia y Mascara de Majora 


        //PRACTICA / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        glm::mat4 model(1);
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
        //Forzamiento del uso de la textura extraída
        // Es el entrelazamiento manual de la textura al modelo
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, compuTextura);
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
        fondo.Draw(lightingShader);                    //cuarto en OBJ     <---


            ////Fondo 1 - - - - - - - - - - - - - - - - - - - - - - - - - -DERECHA
            //lampShader.Use();
            //GLint modelLoc = glGetUniformLocation(lampShader.Program, "model");
            //model = glm::mat4(1.0f);					// <--- REINICIO DE MATRIZ
            //model = glm::translate(model, glm::vec3(9.0f, 0.0f, 0.0f));
            //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            ////
            //glBindTexture(GL_TEXTURE_2D, texture3);
            ////se actualizaa ÚNICAMENTE el uniform 'modelLoc' en el shader para esta nueva posición
            //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            ////
            //glBindVertexArray(VAO);
            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(25 * sizeof(unsigned int)));/////////////////
            //glBindVertexArray(0);

            //prueba de los modelos de luna y sol
            //model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
            //model = glm::translate(model, glm::vec3(-9.0f, 0.0f, -0.5f));
            //model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            //glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            //luna.Draw(lightingShader);
            //model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
            //model = glm::translate(model, glm::vec3(-7.0f, 0.0f, -0.5f));
            //model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            //glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            //sol.Draw(lightingShader);

        // FUENTES DE LUZ = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
        // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
        // Primer cubo de luz = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
        //lampshader.Use();                       //se le manda otro shader
        //glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        //glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, lightPos + movelightPos);
        //model = glm::scale(model, glm::vec3(0.3f));
        //glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);

        // Segundo cubo de luz = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
        //lampshader.Use();
        //glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        //glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        //model = glm::mat4(1.0f);
        //    //model = glm::translate(model, lightPos2 + movelightPos2);                 //posición de la luz 2
        //model = glm::translate(model, glm::vec3(-0.64f + movelightPos2x, 0.7f + movelightPos2y, 2.5f + movelightPos2z));        //op1
        //    //model = glm::translate(model, (lightPos2 + movelightPos2x, lightPos2 + movelightPos2y, lightPos2 + movelightPos2z));  //op2
        //    /*model = glm::translate(model, lightPos2 + movelightPos2x);                                                            //op3
        //    model = glm::translate(model, lightPos2 + movelightPos2y);
        //    model = glm::translate(model, lightPos2 + movelightPos2z);*/
        //            //PArte en la que se aplicaría el movimiento libre de la luz 2
        //model = glm::scale(model, glm::vec3(1.0f, 0.2f, 0.3f));                     //para diferenciarla
        //glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);
        ////sol.Draw(lampshader);                    //    <---


        // LUNA = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
        lampshader.Use();
        model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(movelightRot), glm::vec3(0.0f, 0.0f, 1.0f));        //Punto de giro
        //model = glm::translate(model, glm::vec3(7.0f, 0.0f, 0.0f));                                 //Posision donde estará inicialmente
        model = glm::translate(model, lightPos);                     //Posision donde estará inicialmente
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, lightPos + movelightPos);                 //posición de la luz 2
            ////model = glm::translate(model, glm::vec3(-0.75f + movelightPos2x, 0.7f + movelightPos2y, 1.5f + movelightPos2z));        //op1
        model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAO);
        luna.Draw(lampshader);                          //luna en OBJ     <---

        // SOL  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
        lampshader.Use();
        model = glm::mat4(1.0f);                // <--- REINICIO DE MATRIZ
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(movelightRot), glm::vec3(0.0f, 0.0f, 1.0f));        //Punto de giro
        //model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 0.0f));                                //Posision donde estará inicialmente
        model = glm::translate(model, lightPos2);                                //Posision donde estará inicialmente
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //model = glm::mat4(1.0f);
            ////model = glm::translate(model, lightPos2 + movelightPos);                 //posición de la luz 2
            //model = glm::translate(model, glm::vec3(-0.75f + movelightPos2x, 0.7f + movelightPos2y, 1.5f + movelightPos2z));        //op1
        model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
        glUniformMatrix4fv(glGetUniformLocation(lampshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAO);
        sol.Draw(lampshader);                           //sol en OBJ     <---



        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls_______________________________________________
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

    if (activanim)
    {
        if (rot > -90.0f)
            rot -= 0.1f;
    }

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
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
    //_______________________________________________________________
    if (keys[GLFW_KEY_O])
    {
       
        movelightPos += 0.1f;
    }

    if (keys[GLFW_KEY_L])
    {
        
        movelightPos -= 0.1f;
    }
    //Movimiento para la segunda luz_________________________________
    if (keys[GLFW_KEY_K])       //DERECHA   K
    {
        movelightPos2x += 0.1f;
    }
    if (keys[GLFW_KEY_H])       //IZQUIERDA H
    {
        movelightPos2x -= 0.1f;
    }
    
    if (keys[GLFW_KEY_U])       //ADELANTE  U
    {
        movelightPos2z -= 0.1f;
    }
    if (keys[GLFW_KEY_J])       //ATRAS     J
    {
        movelightPos2z += 0.1f;
    }

    if (keys[GLFW_KEY_I])       //ARRIBA    I
    {
        movelightPos2y += 0.1f;
    }
    if (keys[GLFW_KEY_Y])       //ABAJO     Y
    {
        movelightPos2y -= 0.1f;
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

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
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


