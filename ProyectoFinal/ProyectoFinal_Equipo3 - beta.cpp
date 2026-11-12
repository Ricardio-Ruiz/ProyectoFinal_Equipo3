

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//Proyecto Final - Equipo 3 											  
//                                                          Ruiz Vargas Ricardo
//                                                                    316226068
//                                                   Reyes Sánchez Laura Pamela
//                                                                    318083593
//Fecha de entrega: 11 de noviembre de 2026 							
//
// 

#include <iostream>
#include <cmath>
#include <fstream>

// GLEW & GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Load Models & Textures
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Headers propios
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include "Texture.h"      //skybox

//PAra la musica            Probar, si no funciona, eliminar las 3:
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


// Prototipos
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void saveFramesToFile();
void loadFramesFromFile();
void resetElements();
void interpolation();
// Funciones de animacion del Xolo
void agregarFrameXolo(
    float x, float y, float z,
    float rot,
    float delIzq, float delDer,
    float trasIzq, float trasDer,
    float cabeza, float cuello, float cola,
    float cuerpoY, float cuerpoRot
);

void inicializarXolo();
void interpolarXolo();
void animarXolo();
void dibujarXolo(Shader& shader, GLint modelLoc,
    Model& tronco, Model& cuello, Model& cabeza, Model& cola,
    Model& pataDelIzq, Model& pataDelDer, Model& pataTrasIzq, Model& pataTrasDer);

// Dimensiones y Cámara
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

//Camera camera(glm::vec3(-152.0f, 11.0f, 9.0f)); // Posición inicial
//Camera camera(glm::vec3(-120.0f, 16.0f, 10.0f));
//Camera camera(glm::vec3(250.0f, 16.0f, -50.0f)); // Posición PRUEBA
Camera camera(glm::vec3(-151.0f, 17.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f); //para que mire hacia +x
// 
// Variable global para el tipo de cámara (1 = Primera persona, 2 = Tercera persona, 3 = Libre)
int tipoCamara = 1;                 // 1 = Primera persona, 2 = Tercera persona, 3 = Libre


GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// =========================================================================
// 1. ILUMINACIÓN (1 Direccional, 7 Puntuales, 2 Spotlights)
// =========================================================================
glm::vec3 pointLightPositions[7] = {
    glm::vec3(-102.06f,  13.69f,  -3.6f),
    glm::vec3(-79.73f,  13.43f, 28.57f),
    glm::vec3(130.39f,  13.84f,  -81.05f),
    glm::vec3(344.87f,  20.63f,  -76.34f),
    glm::vec3(-322.7f,  13.0f, -632.9f), //
    glm::vec3(-292.7f,  13.0f, -632.9f),
    glm::vec3(314.04f,  13.0f,  -78.4f)
};

glm::vec3 spotLightPositions[2] = {
    glm::vec3(0.0f, 5.0f, 0.0f),  // Ej. Reflector 1 fijo
    glm::vec3(0.0f, 0.0f, 0.0f)   // Ej. Linterna del jugador (se actualiza en el loop)
};

glm::vec3 spotLightDirections[2] = {
    glm::vec3(0.0f, -1.0f, 0.0f), // Apuntando hacia abajo
    glm::vec3(0.0f,  0.0f, -1.0f) // Apuntando hacia donde mira la cámara
};

// =========================================================================
// 2. VARIABLES DE ANIMACIÓN (3 Tipos)
// =========================================================================

// A) Procedimental (Basada en matemáticas/tiempo)
float waterOffset = 0.0f;
float flotaY = 0.0f;

float trajFloteY = 0.0f;            // Movimiento arriba y abajo
float trajBalanceo = 0.0f;          // Rotación de lado a lado
float trajRecorrido = 0.0f;         // Distancia para las trajineras lejanas

// B) Máquina de Estados (Ej. Ciclo de día/noche o compuertas)
int estadoDiaNoche = 0; // 0=Día, 1=Tarde, 2=Noche
bool activarLinterna = true;

        // Máquina de Estados: Recorrido Trajinera Lejana
glm::vec3 posTrajLejana = glm::vec3(-142.35f, 5.0f, 98.6f); // Punto de inicio
int estadoTrajLejana = 0; // 0 = Hacia punto 1, 1 = Hacia punto 2, 2 = Regreso a inicio

// C) Keyframes (Estructura genérica para personaje principal o un objeto complejo)
#define MAX_FRAMES 200
int i_max_steps = 90;
int i_curr_steps = 0;


float objPosX = -151.0f, objPosY = 0.0f, objPosZ = 8.0f;                                        //SPAWN A inicio
//float objPosX = 81.0f, objPosY = 0.0f, objPosZ = -39.0f;                                       //SPAWN B
//float objPosX = 280.0f, objPosY = 0.0f, objPosZ = -94.0f;                                       //SPAWN C
//float objPosX = 191.0f, objPosY = 0.0f, objPosZ = 75.0f;                                       //SPAWN D
//float objPosX = 294.0f, objPosY = 0.0f, objPosZ = -226.0f;                                       //SPAWN E

float objRotY = 90.0f, art1 = 0.0f, art2 = 0.0f; // articulaciones genéricas

typedef struct _frame {
    float objPosX, objPosY, objPosZ;
    float objRotY;
    float art1, art2;
    // Incrementos
    float incX, incY, incZ;
    float incRotY, incArt1, incArt2;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool playAnim = false;
int playIndex = 0;


// =====================================================
// VARIABLES PARA LA ANIMACION DEL XOLO
// =====================================================

#define XOLO_MAX_FRAMES 80

struct XOLO_FRAME {
    float x, y, z;
    float rotY;

    float pataDelIzq;
    float pataDelDer;
    float pataTrasIzq;
    float pataTrasDer;

    float cabeza;
    float cuello;
    float cola;

    float cuerpoY;
    float cuerpoRot;

    float incX, incY, incZ, incRotY;

    float incPataDelIzq, incPataDelDer;
    float incPataTrasIzq, incPataTrasDer;

    float incCabeza, incCuello, incCola;
    float incCuerpoY, incCuerpoRot;
};

XOLO_FRAME XoloFrames[XOLO_MAX_FRAMES];

int XoloFrameIndex = 0;
int XoloPlayIndex = 0;
int XoloSteps = 0;
int XoloMaxSteps = 240;
int XoloGaitSteps = 0;

bool XoloPlay = false;

// Posicion y articulaciones
float xoloX = 225.0f;
float xoloY = 18.0f;
float xoloZ = 15.0f;

float xoloRotY = 0.0f;

float xoloPataDelIzq = 0.0f;
float xoloPataDelDer = 0.0f;
float xoloPataTrasIzq = 0.0f;
float xoloPataTrasDer = 0.0f;

float xoloCabeza = 0.0f;
float xoloCuello = 0.0f;
float xoloCola = 0.0f;

float xoloCuerpoY = 0.0f;
float xoloCuerpoRot = 0.0f;

// Seguimiento del mariachi: guardamos su trayectoria DESPUES de aplicar
// las colisiones del mapa. El Xolo sigue esa misma trayectoria con retraso.
struct XOLO_TRAIL_POINT {
    glm::vec3 posicion;
};

#define XOLO_MAX_TRAIL 4096
XOLO_TRAIL_POINT XoloTrail[XOLO_MAX_TRAIL];
int XoloTrailCount = 0;
float XoloDistanciaSeguimiento = 7.0f;
float XoloGaitTime = 0.0f;
float XoloSitBlend = 1.0f;
bool XoloTrailReady = false;
glm::vec3 XoloUltimaPosJugador(0.0f);
bool XoloHayPosJugadorAnterior = false;
// =====================================================

GLfloat skyboxVertices[] = {
    // Positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};



// =========================================================================
// MAIN
// =========================================================================
int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final - Xochimilco: Recorrido Virtual del Día de Muertos", nullptr, nullptr);
    if (nullptr == window) { glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Callbacks de Input
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // CÁMARA TIPO VIDEOJUEGO (Cursor atrapado y oculto para libre exploración)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) return EXIT_FAILURE;
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Carga de Shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    //Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader skyboxShader("Shader/SkyBox.vs", "Shader/SkyBox.frag");



    // Configuración del Skybox (VAO y VBO)
    GLuint skyboxVBO, skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // Carga de las 6 texturas del cubo
    vector<const GLchar*> faces;
    faces.push_back("SkyBox/frente3.png");      // 1. Derecha (+X)
    faces.push_back("SkyBox/atras5.png");    // 2. Izquierda (-X)
    faces.push_back("SkyBox/arriba5.png");       // 3. Arriba (+Y)
    faces.push_back("SkyBox/abajo.png");        // 4. Abajo (-Y)
    faces.push_back("SkyBox/izquierda5.png");        // 5. Atrás (+Z)
    faces.push_back("SkyBox/derecha5.png");       // 6. Frente (-Z)

    // NOTA: Asegúrate de que la clase en tu archivo "Texture.h" se llame exactamente así.
    GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);
    std::cout << "ID del Cubemap: " << cubemapTexture << std::endl;






    // =========================================================================
    // CARGA DE MODELOS (Espacio para assets del proyecto)
    // =========================================================================
    Model ChinampaA((char*)"Models/ZonaA/ChinampaA.obj");
        Model caminoA((char*)"Models/ZonaA/caminoA.obj");
        Model arbolA((char*)"Models/ZonaA/vegetacion/arbolesA.obj");
        Model decoA((char*)"Models/ZonaA/deco.obj");
        Model cartel((char*)"Models/ZonaA/cartel.obj");

    Model ChinampaB((char*)"Models/ZonaB/ChinampaB.obj");
        Model plantasB((char*)"Models/ZonaB/vegetacion/plantas.obj");
        Model caminoB((char*)"Models/ZonaB/caminoB.obj");
        Model cosasB((char*)"Models/ZonaB/elementos/cosas.obj");

        // Artesano de papel picado - piezas independientes para modelado jerarquico
        Model mesaArtesano((char*)"Models/Artesano/mesaArtesano.obj");
        Model cuerpoArtesano((char*)"Models/Artesano/cuerpoArtesano.obj");
        Model brazoCincelArtesano((char*)"Models/Artesano/brazoCincelArtesano.obj");
        Model brazoMartilloArtesano((char*)"Models/Artesano/brazoMartilloArtesano.obj");

        Model arbolB((char*)"Models/ZonaB/vegetacion/arboles.obj");

    Model ChinampaC((char*)"Models/ZonaC/ChinampaC.obj");
        Model caminoC((char*)"Models/ZonaC/caminoC.obj");
        Model ofrenda((char*)"Models/ZonaC/ofrenda.obj");
        Model arbolC((char*)"Models/ZonaC/vegetacion/arbolesC.obj");

    Model ChinampaD((char*)"Models/ZonaD/ChinampaD.obj");
        Model caminoD((char*)"Models/ZonaD/caminoD.obj");
        Model arbolD((char*)"Models/ZonaD/vegetacion/arbolesC.obj");
        Model deco((char*)"Models/ZonaD/deco.obj");

    Model ChinampaE((char*)"Models/ZonaE/ChinampaE.obj");
        Model pueblo1((char*)"Models/ZonaE/Pueblo/derecha.obj");
        Model pueblo2((char*)"Models/ZonaE/Pueblo/arriba.obj");
        Model pueblo3((char*)"Models/ZonaE/Pueblo/abajo.obj");
        Model pueblo4((char*)"Models/ZonaE/Pueblo/vegetacion.obj");
        Model cement((char*)"Models/ZonaE/Panteon/panteonE.obj");
        //Model pueblo4((char*)"Models/ZonaE/camino.obj");


    Model Chinampas((char*)"Models/Extras-Relleno/Chinampas.obj");
        Model hierbas((char*)"Models/Extras-Relleno/hierbas.obj");
        Model arbolesExtra((char*)"Models/Extras-Relleno/arboles.obj");
        Model velas((char*)"Models/Extras-Relleno/velas.obj");
        Model picado((char*)"Models/Extras-Relleno/papepicao.obj");

    Model agua((char*)"Models/Rio-Canales/awa.obj");
        Model plantasawa((char*)"Models/Rio-Canales/plantas.obj");
        Model trajineras1((char*)"Models/Rio-Canales/trajineras.obj");
        Model trajineras2((char*)"Models/Rio-Canales/trajineras2.obj");
        Model trajineras3((char*)"Models/Rio-Canales/trajineras3.obj");

    //Model Personaje((char*)"Models/Proyecto/Personaje.");
    // Carga de piezas del personaje
    Model Cuerpo((char*)"Models/Personaje/cuerpo.obj");
    Model BrazoI((char*)"Models/Personaje/brazo-iz.obj");
    Model BrazoD((char*)"Models/Personaje/brazo-de.obj");
    Model PiernaI((char*)"Models/Personaje/pierna-iz.obj");
    Model PiernaD((char*)"Models/Personaje/pierna-de.obj");
    Model zinnia((char*)"Models/Personaje/prueba.obj");

    // =====================================================
    // MODELOS DEL XOLO
    // =====================================================
    Model XoloTronco((char*)"Models/Xolo/XoloTronco.obj");
    Model XoloCuello((char*)"Models/Xolo/xoloCuello.obj");
    Model XoloCabeza((char*)"Models/Xolo/xoloCabeza.obj");
    Model XoloCola((char*)"Models/Xolo/xoloCola.obj");
    Model XoloPataDelIzq((char*)"Models/Xolo/XoloPata_Del_Izq.obj");
    Model XoloPataDelDer((char*)"Models/Xolo/XoloPata_Del_Der.obj");
    Model XoloPataTrasIzq((char*)"Models/Xolo/XoloPata_Tras_Izq.obj");
    Model XoloPataTrasDer((char*)"Models/Xolo/XoloPata_Tras_Der.obj");



    ////- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ////Para el GLB se supone que se debe de cargar el archivo de textura de forma manual
    //// Carga manual de la texxtura de la laptop:
    //unsigned int compuTextura;
    //glGenTextures(1, &compuTextura);
    //glBindTexture(GL_TEXTURE_2D, compuTextura);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //int widthL, heightL, nrChannelsL;
    ////stbi_set_flip_vertically_on_load(true);      // <--- VOLTEA LA IMAGEN
    //unsigned char* dataL = stbi_load("Models/P6/LaptopMouse/compu/Image_0-inception-.jpg", &widthL, &heightL, &nrChannelsL, 0);
    ////    unsigned char* dataL = stbi_load("Models/P6/LaptopMouse/compu/Image_0GG.jpg", &widthL, &heightL, &nrChannelsL, 0);    //Mi pantalla
    //if (dataL) {
    //    GLenum format;
    //    if (nrChannelsL == 1) format = GL_RED;
    //    else if (nrChannelsL == 3) format = GL_RGB;
    //    else if (nrChannelsL == 4) format = GL_RGBA;

    //    glTexImage2D(GL_TEXTURE_2D, 0, format, widthL, heightL, 0, format, GL_UNSIGNED_BYTE, dataL);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else {
    //    std::cout << "Fallo al cargar la textura de la compu" << std::endl;
    //}
    //stbi_image_free(dataL);
    //// Inyección manual a todas las mallas del modelo
    //for (unsigned int i = 0; i < compu.meshes.size(); i++) {
    //    compu.meshes[i].textures.clear();    // <--- ESTA LÍNEA ELIMINA LA TEXTURA NEGRA ROTA POR FIN!!        
    //    Texture texL;
    //    texL.id = compuTextura;
    //    texL.type = "texture_diffuse";
    //    texL.path = "Models/P6/LaptopMouse/compu/Image_0.jpg";
    //    compu.meshes[i].textures.push_back(texL);
    //}
    ////- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


    //Musica
    PlaySoundA("La llorona - Homenaje a Antonio Bribiesca.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    // Abrir el archivo de pasos y asignarle un alias
    mciSendStringA("open \"Footsteps-Grass-Sound-Effect-_HD_.wav\" type waveaudio alias sonidoPasos", NULL, 0, NULL);
    // Calentamiento: reproducir y pausar de inmediato en segundo plano
    mciSendStringA("play sonidoPasos", NULL, 0, NULL);
    mciSendStringA("pause sonidoPasos", NULL, 0, NULL);
    // Abrir el sonido del Xolo
    mciSendStringA(
        "open \"sonidoPerro.mp3\" type mpegvideo alias sonidoPerro",
        NULL, 0, NULL
    );
    // Abrir el sonido del artesano
    mciSendStringA(
        "open \"sonidoArtesano.mp3\" type mpegvideo alias sonidoArtesano",
        NULL, 0, NULL
    );
    // Subir el volumen del sonido del artesano
    mciSendStringA(
        "setaudio sonidoArtesano volume to 500",
        NULL, 0, NULL
    );

    // Inicializar Keyframes
    resetElements();
    // Inicializar keyframes del Xolo
    inicializarXolo();

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();
        Animation();
        animarXolo();

        // Color de fondo (Cambia según estadoDiaNoche)
        //if(estadoDiaNoche == 0) glClearColor(0.5f, 0.8f, 0.9f, 1.0f);      // Día
        //else if(estadoDiaNoche == 1) glClearColor(0.8f, 0.4f, 0.2f, 1.0f); // Tarde
        //else glClearColor(0.05f, 0.05f, 0.1f, 1.0f);                       // Noche
        glClearColor(0.6588f, 0.4431f, 0.0392f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Use();
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // LUCES . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
        //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
        // Luz Direccional (Sol/Luna)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.8f, 0.4f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

        // 7 Luces Puntuales (Antorchas, faroles en trajineras, etc.)
        // crea el factor de parpadeo usando el tiempo
        float tiempoAnim = glfwGetTime();
        float parpadeo = (sin(tiempoAnim * 15.0f) + cos(tiempoAnim * 23.0f) + sin(tiempoAnim * 37.0f)) / 3.0f;
        // parpadeo para que la intensidad varie entre 60% y 100% (evitando  que la vela se apague por completo)
        float intVela = 0.8f + (0.2f * parpadeo);
        for (int i = 0; i < 7; i++) {
            std::string number = std::to_string(i);
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].position").c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].ambient").c_str()), 0.1f * intVela, 0.05f * intVela, 0.02f * intVela);
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].diffuse").c_str()), 0.8f * intVela, 0.4f * intVela, 0.1f * intVela);
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].specular").c_str()), 1.0f * intVela, 0.5f * intVela, 0.2f * intVela);
            glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].constant").c_str()), 1.0f);
                    //ALCANCE DE LA "ESFERA" (Llega al doble de distancia pero se corta abruptamente)
            glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].linear").c_str()), 0.0045f);    // Antes era 0.09
            glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].quadratic").c_str()), 0.0019f); // Antes era 0.032
        }

        // 2 Spotlights
        // Spotlight 0 (Fijo en el escenario)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[0].position"), spotLightPositions[0].x, spotLightPositions[0].y, spotLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[0].direction"), spotLightDirections[0].x, spotLightDirections[0].y, spotLightDirections[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[0].ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[0].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[0].quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[0].cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[0].outerCutOff"), glm::cos(glm::radians(15.0f)));

        // Spotlight 1 (Linterna del jugador atada a la cámara)
        float spot1Intens = activarLinterna ? 1.0f : 0.0f;
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[1].position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[1].direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[1].ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[1].diffuse"), spot1Intens, spot1Intens, spot1Intens);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLights[1].specular"), spot1Intens, spot1Intens, spot1Intens);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[1].linear"), 0.045f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[1].quadratic"), 0.0075f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[1].cutOff"), glm::cos(glm::radians(10.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLights[1].outerCutOff"), glm::cos(glm::radians(15.0f)));

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 3000.0f);        //ver mas
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        
        // DIBUJADO DE MODELOS 
        //Carga de modelo / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        // / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
        glm::mat4 model(1.0f);
        glm::mat4 modelTemp(1.0f);

        // 1. Entorno Estático (Chinampas)

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaA.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                caminoA.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                arbolA.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                decoA.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                cartel.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaB.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                caminoB.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                plantasB.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                cosasB.Draw(lightingShader);

                // =============================================================
                // ARTESANO cINCELANDO PAPEL PICADO EN ZONA B
                // Modelado jerarquico: mesa y torso como base; brazos como hijos
                // =============================================================
                glm::mat4 modeloArtesano = glm::mat4(1.0f);
                modeloArtesano = glm::translate(modeloArtesano, glm::vec3(140.0f, 12.5f, -80.0f));
                modeloArtesano = glm::rotate(modeloArtesano, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modeloArtesano = glm::scale(modeloArtesano, glm::vec3(5.0f));

                // Mesa fija: elemento base del puesto de trabajo.
                model = modeloArtesano;
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                mesaArtesano.Draw(lightingShader);

                // Torso fijo: padre de los dos brazos.
                model = modeloArtesano;
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                cuerpoArtesano.Draw(lightingShader);

                // Ciclo repetitivo de trabajo. Solo controla las articulaciones del artesano.
                float tiempoTrabajoArtesano = (float)glfwGetTime();
                float golpeMartillo = sinf(tiempoTrabajoArtesano * 10.0f);
                float ajusteCincel = sinf(tiempoTrabajoArtesano * 6.0f + 0.45f);

                // Brazo del cincel: movimiento corto para mantener el cincel sobre el papel.
                // El pivote se expresa en las coordenadas locales originales del OBJ.
                glm::vec3 pivoteCincel(0.02f, 1.35f, -0.20f);
                glm::mat4 modeloBrazoCincel = modeloArtesano;
                modeloBrazoCincel = glm::translate(modeloBrazoCincel, pivoteCincel);
                modeloBrazoCincel = glm::rotate(modeloBrazoCincel, glm::radians(4.0f * ajusteCincel), glm::vec3(0.0f, 0.0f, 1.0f));
                modeloBrazoCincel = glm::translate(modeloBrazoCincel, -pivoteCincel);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modeloBrazoCincel));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                brazoCincelArtesano.Draw(lightingShader);

                // Brazo del martillo: recorrido mayor para simular el golpe.
                glm::vec3 pivoteMartillo(-0.30f, 1.38f, -0.24f);
                glm::mat4 modeloBrazoMartillo = modeloArtesano;
                modeloBrazoMartillo = glm::translate(modeloBrazoMartillo, pivoteMartillo);
                modeloBrazoMartillo = glm::rotate(modeloBrazoMartillo, glm::radians(17.0f * golpeMartillo), glm::vec3(0.0f, 0.0f, 1.0f));
                modeloBrazoMartillo = glm::translate(modeloBrazoMartillo, -pivoteMartillo);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modeloBrazoMartillo));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                brazoMartilloArtesano.Draw(lightingShader);

                // Restaurar la matriz base y el estado de transparencia para los modelos siguientes.
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                //glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        
        
                // =============================================================
                // SONIDO AMBIENTAL DEL ARTESANO SEGUN LA DISTANCIA
                // =============================================================

                static bool sonidoArtesanoActivo = false;

                // Posición del artesano
                glm::vec3 posicionArtesano(140.0f, 12.5f, -80.0f);

                // Posición actual del jugador
                glm::vec3 posicionJugador = camera.GetPosition();

                // Calcular distancia entre el jugador y el artesano
                float distanciaArtesano = glm::distance(
                    posicionJugador,
                    posicionArtesano
                );

                // Distancia máxima para escuchar el sonido
                float distanciaMaximaArtesano = 60.0f;

                // Si el jugador se acerca, iniciar el sonido
                if (distanciaArtesano <= distanciaMaximaArtesano &&
                    !sonidoArtesanoActivo)
                {
                    mciSendStringA(
                        "seek sonidoArtesano to start",
                        NULL, 0, NULL
                    );

                    mciSendStringA(
                        "play sonidoArtesano repeat",
                        NULL, 0, NULL
                    );

                    sonidoArtesanoActivo = true;
                }

                // Si el jugador se aleja, detener el sonido
                else if (distanciaArtesano > distanciaMaximaArtesano &&
                    sonidoArtesanoActivo)
                {
                    mciSendStringA(
                        "stop sonidoArtesano",
                        NULL, 0, NULL
                    );

                    sonidoArtesanoActivo = false;
                }

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                arbolB.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaC.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                caminoC.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                ofrenda.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                arbolC.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaD.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                caminoD.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                arbolD.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                deco.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaE.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                pueblo1.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                pueblo2.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                pueblo3.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                pueblo4.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                cement.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        //// Ofrenda de la Zona C
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        //OfrendaZonaC.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        Chinampas.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                hierbas.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                arbolesExtra.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                velas.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                    //ON al viento y mandamos el tiempo
                glUniform1i(glGetUniformLocation(lightingShader.Program, "isPapelPicado"), 1);
                glUniform1f(glGetUniformLocation(lightingShader.Program, "time"), glfwGetTime());
                picado.Draw(lightingShader);
                    //OFF al viento y limpiamos la transparencia
                glUniform1i(glGetUniformLocation(lightingShader.Program, "isPapelPicado"), 0);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);


        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        agua.Draw(lightingShader);*/



        // 2. Animación Procedimental (Agua moviéndose)==================================
        model = glm::mat4(1.0f);
        // Usamos 'flotaY' en el eje Y para que todo el plano suba y baje suavemente
        model = glm::translate(model, glm::vec3(0.0f, flotaY * 0.8f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1); // Indicador de agua
        // Enviamos waterOffset al shader para deslizar la textura (UV Scrolling)
        glUniform1f(glGetUniformLocation(lightingShader.Program, "uvOffset"), waterOffset);
        agua.Draw(lightingShader);
        // Reseteamos el uvOffset a 0.0f para que no afecte a las chinampas ni al mariachi
        glUniform1f(glGetUniformLocation(lightingShader.Program, "uvOffset"), 0.0f);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
                plantasawa.Draw(lightingShader);
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f + trajFloteY, 0.0f));              //sube baja
                model = glm::rotate(model, glm::radians(trajBalanceo), glm::vec3(0.0f, 0.0f, 1.0f)); // Balanceo lateral
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                trajineras1.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, trajFloteY, 0.0f));              //sube baja
                //model = glm::rotate(model, glm::radians(trajBalanceo), glm::vec3(0.0f, 0.0f, 1.0f)); // Balanceo lateral
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                trajineras2.Draw(lightingShader);

                model = glm::mat4(1.0f);
                model = glm::translate(model, posTrajLejana + glm::vec3(0.0f, 0.0f , 0.0f));
                if (estadoTrajLejana == 0) model = glm::rotate(model, glm::radians(330.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                else if (estadoTrajLejana == 1) model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                else if (estadoTrajLejana == 2) model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
                trajineras3.Draw(lightingShader);

        //// 3. Animación Procedimental (Trajinera flotando)
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(5.0f, flotaY, -5.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        //Trajinera.Draw(lightingShader);

        //// 4. Animación por Keyframes (Personaje Principal Jerárquico)
        //modelTemp = model = glm::mat4(1.0f);
        //modelTemp = model = glm::translate(model, glm::vec3(objPosX, objPosY, objPosZ));
        //modelTemp = model = glm::rotate(model, glm::radians(objRotY), glm::vec3(0.0f, 1.0f, 0.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //Personaje.Draw(lightingShader);
        //
        //// Brazo del Personaje (Hijo del cuerpo)
        //model = modelTemp;
        //model = glm::translate(model, glm::vec3(0.5f, 1.2f, 0.0f)); // Offset del hombro
        //model = glm::rotate(model, glm::radians(art1), glm::vec3(1.0f, 0.0f, 0.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //Articulacion1.Draw(lightingShader);


        // =========================================================================
        // DIBUJADO DEL PERSONAJE JERÁRQUICO

        // Condición: Si es 1ra persona (1) Y la cámara mira un poco hacia arriba (> 0.05f)
        bool ocultarPorSombrero = (tipoCamara == 1 && camera.GetFront().y > 0.05f || tipoCamara == 1 && keys[GLFW_KEY_S] && camera.GetFront().y < 0.05f);

        // Solo dibujamos al mariachi si NO debemos ocultarlo
        if (!ocultarPorSombrero) {
            model = glm::mat4(1.0f);
            modelTemp = glm::mat4(1.0f);
            modelTemp = glm::translate(modelTemp, glm::vec3(objPosX, objPosY - 2.2f, objPosZ));
            // rotar el cuepro hacia donde mira la cámara
            modelTemp = glm::rotate(modelTemp, glm::radians(objRotY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTemp = glm::rotate(modelTemp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTemp = glm::scale(modelTemp, glm::vec3(5.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTemp));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Cuerpo.Draw(lightingShader);

            // 1. BRAZO IZQUIERDO (Hijo del Torso, rota desde el hombro)
            glm::mat4 modelBrazoIzq = modelTemp;
            modelBrazoIzq = glm::translate(modelBrazoIzq, glm::vec3(0.0f, 0.3f, -0.12f)); // Coordenada aproximada del hombro izquierdo
            modelBrazoIzq = glm::rotate(modelBrazoIzq, glm::radians(art1), glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBrazoIzq));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            BrazoI.Draw(lightingShader);

            // 2. BRAZO DERECHO (Hijo del Torso, rota inverso)
            glm::mat4 modelBrazoDer = modelTemp;
            modelBrazoDer = glm::translate(modelBrazoDer, glm::vec3(0.0f, 0.3f, 0.12f)); // Coordenada aproximada del hombro derecho
            modelBrazoDer = glm::rotate(modelBrazoDer, glm::radians(art2), glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBrazoDer));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            BrazoD.Draw(lightingShader);

            // 3. PIERNA IZQUIERDA (Hija del Torso, rota desde la cadera)
            glm::mat4 modelPiernaIzq = modelTemp;
            modelPiernaIzq = glm::translate(modelPiernaIzq, glm::vec3(0.0f, 0.0f, 0.0f)); // Coordenada aproximada de la cadera
            modelPiernaIzq = glm::rotate(modelPiernaIzq, glm::radians(art2), glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPiernaIzq));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            PiernaI.Draw(lightingShader);

            // 4. PIERNA DERECHA (Hija del Torso)
            glm::mat4 modelPiernaDer = modelTemp;
            modelPiernaDer = glm::translate(modelPiernaDer, glm::vec3(-0.0f, 0.0f, 0.0f)); // Coordenada aproximada de la cadera
            modelPiernaDer = glm::rotate(modelPiernaDer, glm::radians(art1), glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPiernaDer));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            PiernaD.Draw(lightingShader);

            ////personaje prueba
            //model = glm::mat4(1.0f);
            ////model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            ////model = glm::scale(model, glm::vec3(3.0f, 2.0f, 2.5f));
            ////model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            //glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            //zinnia.Draw(lightingShader);
        }
        // =========================================================================





        // =========================================================================
        // DIBUJADO DEL XOLO (animacion independiente por keyframes)
        // =========================================================================
        dibujarXolo(lightingShader, modelLoc,
            XoloTronco, XoloCuello, XoloCabeza, XoloCola,
            XoloPataDelIzq, XoloPataDelDer,
            XoloPataTrasIzq, XoloPataTrasDer);

        // =========================================================================
        // DIBUJADO DEL SKYBOX 
        // =========================================================================
        glDepthFunc(GL_LEQUAL); // Cambia la función de profundidad para asegurar que el cielo se renderice al fondo
        skyboxShader.Use();

        // Quitar la traslación de la vista para que el cielo siempre siga a la cámara
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // --- EL PARCHE DEFINITIVO: EVITAR EL CHOQUE DE TEXTURAS ---
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0); // 1. Desenchufamos cualquier textura 2D basura que hayan dejado los modelos

        glActiveTexture(GL_TEXTURE15); // 2. Nos mudamos a un canal completamente libre (el 15)
        glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 15); // Le avisamos al shader que lea el canal 15
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // 3. Enlazamos el cubo limpio
        // ----------------------------------------------------------

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Regresa la profundidad a su estado normal para el siguiente frame


        glfwSwapBuffers(window);
    }
    mciSendStringA("close sonidoPerro", NULL, 0, NULL);
    mciSendStringA("close sonidoArtesano", NULL, 0, NULL);
    glfwTerminate();
    return 0;;
}

// =========================================================================
// LÓGICA DE ACTUALIZACIÓN DE ANIMACIONES
// =========================================================================
void Animation() {
    //Animacion personaje
    // Si se presionó alguna tecla de movimiento, oscilamos los brazos/piernas
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_S] || keys[GLFW_KEY_A] || keys[GLFW_KEY_D]) {
        float cicloCaminata = sin(glfwGetTime() * 12.0f) * 35.0f; // Amplitud de 35 grados
        art1 = cicloCaminata;               // Brazo/Pierna Izquierda
        art2 = -cicloCaminata;              // Brazo/Pierna Derecha (en dirección opuesta)
    }
    else {
        // Regresar suavemente a cero cuando se detenga
        art1 = 0.0f;
        art2 = 0.0f;
    }



    // 1. Animaciones Procedimentales
            //
    //waterOffset = sin(glfwGetTime() * 0.5f) * 0.2f;
    waterOffset = glfwGetTime() * -0.03f;
    //velocidad sube/baja  |/     |/  altura
    flotaY = sin(glfwGetTime() * 3.0f) * 0.3f;
    //flotaY = sin(glfwGetTime() * 1.2f) * 0.05f;

    //trajioneras
    trajFloteY = sin(glfwGetTime() * 2.0f) * 0.15f;
    trajBalanceo = cos(glfwGetTime() * 1.5f) * 2.5f;
    trajRecorrido = sin(glfwGetTime() * 0.1f) * 100.0f;
    // --------------------------------------


    // MÁQUINA DE ESTADOS: TRAJINERA LEJANA AQUÍ:
    glm::vec3 punto0(-142.35f, 5.0f, 90.6f);
    glm::vec3 punto1(790.9f, 5.0f, 380.3f);
    glm::vec3 punto2(-158.48f, 5.0f, 663.39f);

    glm::vec3 destinoActual;
    if (estadoTrajLejana == 0) destinoActual = punto1;
    else if (estadoTrajLejana == 1) destinoActual = punto2;
    else if (estadoTrajLejana == 2) destinoActual = punto0;

    glm::vec3 direccionTraj = glm::normalize(destinoActual - posTrajLejana);
    float velocidadTraj = 12.0f * deltaTime;
    posTrajLejana += direccionTraj * velocidadTraj;

    if (glm::distance(posTrajLejana, destinoActual) < 1.0f) {
        estadoTrajLejana++;
        if (estadoTrajLejana > 2) estadoTrajLejana = 0;
    }


    // 2. Animación Keyframes
    if (playAnim) {
        if (i_curr_steps >= i_max_steps) {
            playIndex++;
            if (playIndex > FrameIndex - 2) {
                playIndex = 0;
                playAnim = false;
            }
            else {
                i_curr_steps = 0;
                interpolation();
            }
        }
        else {
            objPosX += KeyFrame[playIndex].incX;
            objPosY += KeyFrame[playIndex].incY;
            objPosZ += KeyFrame[playIndex].incZ;
            objRotY += KeyFrame[playIndex].incRotY;
            art1 += KeyFrame[playIndex].incArt1;
            art2 += KeyFrame[playIndex].incArt2;
            i_curr_steps++;
        }
    }

}



// =========================================================================
// INPUTS Y CÁMARA
// =========================================================================
//void DoMovement() {                                                         //Cam libre
//    // Cámara WASD
//    if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
//    if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
//
//    // Grabación Keyframes (Controles para mover el modelo genérico)
//    if (keys[GLFW_KEY_UP]) objPosZ -= 2.0f * deltaTime;
//    if (keys[GLFW_KEY_DOWN]) objPosZ += 2.0f * deltaTime;
//    if (keys[GLFW_KEY_LEFT]) objPosX -= 2.0f * deltaTime;
//    if (keys[GLFW_KEY_RIGHT]) objPosX += 2.0f * deltaTime;
//    if (keys[GLFW_KEY_U]) art1 += 50.0f * deltaTime;
//    if (keys[GLFW_KEY_J]) art1 -= 50.0f * deltaTime;
//
//    // Archivos de Animación
//    if (keys[GLFW_KEY_F1]) saveFramesToFile();
//    if (keys[GLFW_KEY_F2]) loadFramesFromFile();
//}

// Variable global para el tipo de cámara (1 = Primera persona, 2 = Tercera persona, 3 = Libre)
//int tipoCamara = 1;

void DoMovement() {                                                         //Cam 1ra perdsona
    // Si el usuario selecciona Camara Libre (Modo 3), usamos el movimiento libre de la cámara
    if (tipoCamara == 3) {
        if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
        if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
        return; // Salimos para que no mueva al personaje ni aplique colisiones de juego
    }

    // 1. Guardar la posición ANTERIOR del personaje
    float oldX = objPosX;
    float oldZ = objPosZ;

    float velocidad = 15.0f * deltaTime;

    // Obtener hacia dónde mira la cámara para que el personaje camine en esa dirección
    glm::vec3 frontDir = camera.GetFront();
    frontDir.y = 0.0f; // Mantener plano en el suelo (XZ)
    frontDir = glm::normalize(frontDir);

    /*    glm::vec3 rightDir = camera.GetRight();
        rightDir.y = 0.0f;
        rightDir = glm::normalize(rightDir);*/

        // Calcula el vector derecho perpendicular en el plano horizontal
    glm::vec3 rightDir = glm::cross(frontDir, glm::vec3(0.0f, 1.0f, 0.0f));
    rightDir = glm::normalize(rightDir);

    //Guarda la posicion ANTES de movernos (para poder revertirla si chocamos)
    //glm::vec3 oldPos = camera.GetPosition();
//Movimiento WASD
/*        if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
        if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);*/
        // Movimiento WASD aplicado al PERSONAJE (objPosX y objPosZ)
    bool moviendo = false;
    glm::vec3 dirMovimiento(0.0f);

    if (keys[GLFW_KEY_W]) { objPosX += frontDir.x * velocidad; objPosZ += frontDir.z * velocidad; dirMovimiento += frontDir; moviendo = true; }
    if (keys[GLFW_KEY_S]) { objPosX -= frontDir.x * velocidad; objPosZ -= frontDir.z * velocidad; dirMovimiento -= frontDir; moviendo = true; }
    if (keys[GLFW_KEY_A]) { objPosX -= rightDir.x * velocidad; objPosZ -= rightDir.z * velocidad; dirMovimiento -= rightDir; moviendo = true; }
    if (keys[GLFW_KEY_D]) { objPosX += rightDir.x * velocidad; objPosZ += rightDir.z * velocidad; dirMovimiento += rightDir; moviendo = true; }


    // Hacer que el monito gire hacia la direccion en la que se esta moviendo jajaja
    static int contadorPasos = 0; // Declarar el contador de sonido
    int umbralPasos = 40;
    static bool estabaMoviendose = false; // Variable para detectar el cambio
    if (moviendo) {
        dirMovimiento = glm::normalize(dirMovimiento);
        objRotY = glm::degrees(atan2(dirMovimiento.x, dirMovimiento.z));


        // Reproducir sonido de pasos a un ritmo controlado
        contadorPasos++;
        if (contadorPasos >= umbralPasos) {
            // Reproducir usando el alias registrado
            // Cerramos cualquier instancia previa antes de reproducir para evitar solapamientos o atascos
            mciSendStringA("play sonidoPasos from 0", NULL, 0, NULL);
            contadorPasos = 0;
        }
        estabaMoviendose = true; // Marcamos que estamos en movimiento
    }
    else {
        // Reiniciar el contador de inmediato al detenerse
        // En cuanto te detienes: reiniciamos el contador y mandamos la orden de PARAR el audio inmediatamente
        contadorPasos = 0;
        // ¡LA CLAVE!: Solo mandamos la orden de STOP una sola vez justo cuando el usuario se detiene
        if (estabaMoviendose) {
            mciSendStringA("stop sonidoPasos", NULL, 0, NULL);
            estabaMoviendose = false; // Reseteamos el estado
        }
    }


    // 2. Revisar a dónde intentó moverse el personaje
    float fX = objPosX;
    float fZ = objPosZ;

    ////revisa a donde intentoo moverse la cámara
    //glm::vec3 newPos = camera.GetPosition();
    //float fX = newPos.x;
    //float fZ = newPos.z;

    bool zonaValida = false;
    float alturaSuelo = objPosY; // Altura por defecto


    // MAPEO ============================================================================
    // ---- ZONASD E LA CHINAMPA A --- - - - - - - - - - - - - - - - - - - - - - - - - - 
    //  Chinampa A (Blanco)
    if (fX >= -122.0f && fX <= -55.0f && fZ >= -9.0f && fZ <= 32.0f) {
        zonaValida = true;
        alturaSuelo = 17.5f;
    }
    // Muelle Rojo - - - - - 
    else if (fX >= -154.0f && fX <= -121.0f && fZ >= 7.0f && fZ <= 10.5f) {
        zonaValida = true;
        alturaSuelo = 10.8f;
    }
    // Muelle Azul 
    else if (fX >= -139.0f && fX <= -134.0f && fZ >= -2.5f && fZ <= 7.5f) {
        zonaValida = true;
        alturaSuelo = 10.8f;
    }
    // Puente Amarillo 
    else if (fX >= -59.0f && fX <= -27.0f && fZ >= 6.0f && fZ <= 12.0f) {
        zonaValida = true;
        alturaSuelo = 19.0f;
    }

    // --- ZONAS DE LA CHINAMPA B --- - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Chinampa B (Rojo)
    else if (fX >= -27.8f && fX <= 155.3f && fZ >= -105.0f && fZ <= 12.0f) {
        zonaValida = true;
        alturaSuelo = 20.0f; // Altura promedio de la Chinampa B
    }
    // Chinampa B - Parte faltante (Rojo en la segunda imagen)
            // debe estar más o menos en: X: 11 a 31, Z: 10 a 35 (para tocar la principal).
    else if (fX >= 0.0f && fX <= 31.0f && fZ >= 12.0f && fZ <= 30.0f) {
        zonaValida = true;
        alturaSuelo = 17.5f; // Misma altura que la principal para fluidez
    }
    //Parte baja 1 (Azul)
    else if (fX >= 185.0f && fX <= 198.8f && fZ >= -130.4f && fZ <= -114.5f) {
        zonaValida = true;
        alturaSuelo = 17.5f;
    }
    //Parte baja 2 (Amarillo)
    else if (fX >= 110.0f && fX <= 185.0f && fZ >= -132.1f && fZ <= -105.0f) {          // Faltsa conectar
        zonaValida = true;
        alturaSuelo = 17.5f;
    }
    //Muelle parte 1 (Blanco en la primera imagen) - - - - - 
    else if (fX >= 5.5f && fX <= 11.5f && fZ >= 30.0f && fZ <= 69.7f) {
        zonaValida = true;
        alturaSuelo = 10.8f;
    }
    //Muelle parte 4 (Azul en la segunda imagen)
    else if (fX >= 11.0f && fX <= 30.5f && fZ >= 31.0f && fZ <= 40.5f) {
        zonaValida = true;
        alturaSuelo = 10.8f;
    }
    //Muelle parte 3 (Amarillo en la segunda imagen)
    else if (fX >= -3.0f && fX <= 7.0f && fZ >= 46.5f && fZ <= 50.5f) {
        zonaValida = true;
        alturaSuelo = 10.8f;
    }
    //Muelle parte 2 (Blanco en la segunda imagen)
    else if (fX >= 11.0f && fX <= 20.5f && fZ >= 59.5f && fZ <= 63.5f) {
        zonaValida = true;
        alturaSuelo = 10.8f;
    }

    // --- ZONAS DE LA CHINAMPA C --- - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Puente Central (Conecta Chinampa B con Chinampa C)
    else if (fX >= 154.0f && fX <= 228.0f && fZ >= -74.0f && fZ <= -66.0f) {
        zonaValida = true;
        alturaSuelo = 21.8f; // Asumiendo altura similar a la Chinampa B
    }
    // Chinampa C (Superior Derecha)
    else if (fX >= 228.0f && fX <= 337.0f && fZ >= -118.0f && fZ <= -27.0f) {
        zonaValida = true;
        alturaSuelo = 20.0f;
    }
    // Puente norte (Conecta Chinampa C con Tierra E)
    else if (fX >= 288.0f && fX <= 299.0f && fZ >= -150.0f && fZ <= -118.0f) {
        zonaValida = true;
        alturaSuelo = 21.8f;
    }
    else if (fX >= 288.0f && fX <= 299.0f && fZ >= -180.0f && fZ <= -150.0f) {
        zonaValida = true;
        alturaSuelo = 20.9f;
    }

    // --- ZONAS DE LA CHINAMPA D --- - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Puente a Chinampa D (Diagonal, aproximado como rectángulo) 
    else if (fX >= 235.0f && fX <= 249.0f && fZ >= -27.0f && fZ <= -20.0f) {
        zonaValida = true;
        alturaSuelo = 21.0f; // Asumiendo bajada
    }
    else if (fX >= 228.0f && fX <= 245.0f && fZ >= -21.0f && fZ <= -5.0f) {
        zonaValida = true;
        alturaSuelo = 22.0f; // Asumiendo bajada
    }
    //Chinampa D - Parte Alta (Inferior Derecha)
    else if (fX >= 212.0f && fX <= 238.0f && fZ >= -5.2f && fZ <= 52.0f) {
        zonaValida = true;
        alturaSuelo = 23.0f;
    }
    //chinampa D - Parte Interno (Conecta zona alta y baja de la isla)
    else if (fX >= 133.0f && fX <= 238.0f && fZ >= 46.0f && fZ <= 90.0f) {
        zonaValida = true;
        alturaSuelo = 23.0f; // Bajando a la zona inferior
    }
    //chinampa D - Parte Baja (Cuadrado CEntral)
    else if (fX >= 120.0f && fX <= 238.0f && fZ >= 89.0f && fZ <= 149.0f) {
        zonaValida = true;
        alturaSuelo = 23.0f;
    }
    // chinampa D (Conecta hacia la parte inferior)
    else if (fX >= 145.0f && fX <= 175.0f && fZ >= 148.0f && fZ <= 172.0f) {
        zonaValida = true;
        alturaSuelo = 22.35f;
    }
    // chinampa D Parte inferior(La base roja más baja)
    else if (fX >= 150.0f && fX <= 218.0f && fZ >= 172.0f && fZ <= 182.0f) {
        zonaValida = true;
        alturaSuelo = 21.9f;
    }
    // P uente entre B y D (Arriba a la derecha, saliendo de Chinampa C)
    else if (fX >= 120.0f && fX <= 140.0f && fZ >= 12.0f && fZ <= 32.0f) {
        zonaValida = true;
        alturaSuelo = 21.8f;
    }
    else if (fX >= 134.0f && fX <= 149.0f && fZ >= 28.0f && fZ <= 48.0f) {
        zonaValida = true;
        alturaSuelo = 23.5f;
    }

    // --- ZONAS DE LA TIERRA E --- - - - - - - - - - - - - - - - - - - - - - - - - - -
    //muelle a C
    else if (fX >= 288.0f && fX <= 299.0f && fZ >= -222.0f && fZ <= -180.0f) {
        zonaValida = true;
        alturaSuelo = 20.2f;
    }
    //muelle lateral
    else if (fX >= 299.0f && fX <= 321.0f && fZ >= -215.0f && fZ <= -208.0f) {
        zonaValida = true;
        alturaSuelo = 13.0f;
    }
    // Tierra E camino
    else if (fX >= 255.0f && fX <= 340.0f && fZ >= -450.0f && fZ <= -222.0f) {
        zonaValida = true;
        alturaSuelo = 20.0f;
    }
    else if (fX >= 85.0f && fX <= 255.0f && fZ >= -450.0f && fZ <= -370.0f) {
        zonaValida = true;
        alturaSuelo = 20.9f;
    }
    else if (fX >= -30.0f && fX <= 86.0f && fZ >= -406.0f && fZ <= -370.0f) {
        zonaValida = true;
        alturaSuelo = 21.2f;
    }
    else if (fX >= -155.0f && fX <= -30.0f && fZ >= -460.0f && fZ <= -370.0f) {
        zonaValida = true;
        alturaSuelo = 21.2f;
    }
    else if (fX >= -312.0f && fX <= -155.0f && fZ >= -460.0f && fZ <= -350.0f) {
        zonaValida = true;
        alturaSuelo = 21.2f;
    }
    // ==================================================================================

    // 3. Aplicar colisiones al personaje
    if (zonaValida) {
        // Si la zona es válida, alineamos la altura Y del personaje con el suelo de esa zona
        objPosY = alturaSuelo;
    }
    else {
        // Si intenta salir de las zonas válidas (choca con agua/vacío), revertimos al personaje
        objPosX = oldX;
        objPosZ = oldZ;
    }

    // 4. Actualizar la posición de la cámara según el modo activo (1ra, 3ra o Libre)
    if (tipoCamara == 1) {
        // Primera Persona: La cámara se coloca en la cabeza/ojos del personaje
        //camera.position = glm::vec3(objPosX + 0.8, objPosY + 0.6f, objPosZ);
            //camera.position = glm::vec3(objPosX + 0.8, (camera.GetFront().y * + 0.6f), objPosZ);
        //camera.position = glm::vec3(objPosX + 0.8, objPosY + 0.6f + (camera.GetFront().y * 0.5f), objPosZ);
        //camera.position = glm::vec3(objPosX, objPosY + 0.6f + (camera.GetFront().y * 0.5f), objPosZ);

        // 1. Distancia desde el origen (0,0,0) del torso hasta la cara del mariachi
        float offsetCaraX = 0.8f; // El valor que te funcionaba antes
        float offsetCaraY = 0.6f; // La altura que te funcionaba antes

        // 2. Calculamos las coordenadas reales de la cara rotando el offsetX con objRotY
        // (Convertimos a radianes y usamos Seno/Coseno para mover el punto en círculo)
        float radianes = glm::radians(objRotY - 90.0f); // -90.0f compensa el offset inicial del modelo

        float posCaraX = objPosX + (cos(radianes) * offsetCaraX);
        float posCaraZ = objPosZ - (sin(radianes) * offsetCaraX);
        float posCaraY = objPosY + offsetCaraY + (camera.GetFront().y * 0.5f); // Se ajusta al ver hacia abajo

        // 3. Posicionamos la cámara exactamente en esas coordenadas calculadas
        camera.position = glm::vec3(posCaraX, posCaraY, posCaraZ);
    }
    else if (tipoCamara == 2) {
        // Tercera Persona: La cámara se coloca atrás y arriba del personaje
        //glm::vec3 offsetCamara = camera.GetFront() * -7.0f + glm::vec3(0.0f, 2.0f, 0.0f);
        float distanciaAtras = 7.0f;
        glm::vec3 offsetCamara = camera.GetFront() * -distanciaAtras + glm::vec3(0.0f, 2.0f, 0.0f);
        camera.position = glm::vec3(objPosX, objPosY, objPosZ) + offsetCamara;
    }
    // Si tipoCamara == 3 (Libre), puedes dejar que la cámara se mueva con su lógica libre original.





    ////aplicar colisiones a la camara - - - - - - - - - - - - - - - - - - - - - - - - - - 
    //if (zonaValida) {
    //    // La camara esta en un piso seguro. se le forza su altura Y para que "camine"
    //    // sumar +2.0f para simular la altura de los ojos.
    //    camera.position.y = alturaSuelo + 2.0f;
    //}
    //else {
    //    // Si intenta salir de las cajas invisibles, se le reverte el movimiento (como un Muro invisible)
    //    camera.position = oldPos;
    //}



}




void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if (!playAnim && FrameIndex > 1) {
            resetElements();
            interpolation();
            playAnim = true;
            playIndex = 0;
            i_curr_steps = 0;
        }
        else {
            playAnim = false;
        }
    }

    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        if (FrameIndex < MAX_FRAMES) {
            KeyFrame[FrameIndex].objPosX = objPosX;
            KeyFrame[FrameIndex].objPosY = objPosY;
            KeyFrame[FrameIndex].objPosZ = objPosZ;
            KeyFrame[FrameIndex].objRotY = objRotY;
            KeyFrame[FrameIndex].art1 = art1;
            KeyFrame[FrameIndex].art2 = art2;
            FrameIndex++;
            std::cout << "Frame guardado: " << FrameIndex << std::endl;
        }
    }

    // El Xolo sigue automaticamente al mariachi en primera/tercera persona.

    // Máquina de estados: Linterna
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        activarLinterna = !activarLinterna;
    }


    //Imprimee coordenadas en la consola (boton P) para mapear la zona <---
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        std::cout << "Borde -> X: " << camera.GetPosition().x
            << " | Y: " << camera.GetPosition().y
            << " | Z: " << camera.GetPosition().z << std::endl;
    }


    // Cambiar modo de cámara con la tecla C
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        tipoCamara++;
        if (tipoCamara > 3) tipoCamara = 1; // Cicla de 1 a 3

        if (tipoCamara == 1) std::cout << "Camara: Primera Persona" << std::endl;
        else if (tipoCamara == 2) std::cout << "Camara: Tercera Persona" << std::endl;
        else if (tipoCamara == 3) std::cout << "Camara: Libre" << std::endl;
    }

}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;
    camera.ProcessMouseMovement(xOffset, yOffset);
}

// =========================================================================
// SISTEMA DE KEYFRAMES (Interpolación y Archivos)
// =========================================================================
void interpolation() {
    KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].objPosX - KeyFrame[playIndex].objPosX) / i_max_steps;
    KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].objPosY - KeyFrame[playIndex].objPosY) / i_max_steps;
    KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].objPosZ - KeyFrame[playIndex].objPosZ) / i_max_steps;
    KeyFrame[playIndex].incRotY = (KeyFrame[playIndex + 1].objRotY - KeyFrame[playIndex].objRotY) / i_max_steps;
    KeyFrame[playIndex].incArt1 = (KeyFrame[playIndex + 1].art1 - KeyFrame[playIndex].art1) / i_max_steps;
    KeyFrame[playIndex].incArt2 = (KeyFrame[playIndex + 1].art2 - KeyFrame[playIndex].art2) / i_max_steps;
}

void resetElements() {
    if (FrameIndex > 0) {
        objPosX = KeyFrame[0].objPosX;
        objPosY = KeyFrame[0].objPosY;
        objPosZ = KeyFrame[0].objPosZ;
        objRotY = KeyFrame[0].objRotY;
        art1 = KeyFrame[0].art1;
        art2 = KeyFrame[0].art2;
    }
}

void saveFramesToFile() {
    std::ofstream file("animacion_proyecto.txt");
    if (file.is_open()) {
        file << FrameIndex << "\n";
        for (int i = 0; i < FrameIndex; i++) {
            file << KeyFrame[i].objPosX << " " << KeyFrame[i].objPosY << " " << KeyFrame[i].objPosZ << " "
                << KeyFrame[i].objRotY << " " << KeyFrame[i].art1 << " " << KeyFrame[i].art2 << "\n";
        }
        file.close();
        std::cout << "Animacion guardada." << std::endl;
    }
}

void loadFramesFromFile() {
    std::ifstream file("animacion_proyecto.txt");
    if (file.is_open()) {
        file >> FrameIndex;
        for (int i = 0; i < FrameIndex; i++) {
            file >> KeyFrame[i].objPosX >> KeyFrame[i].objPosY >> KeyFrame[i].objPosZ
                >> KeyFrame[i].objRotY >> KeyFrame[i].art1 >> KeyFrame[i].art2;
        }
        file.close();
        resetElements();
        std::cout << "Animacion cargada." << std::endl;
    }
}

// =========================================================================
// ANIMACION DEL XOLO POR KEYFRAMES
// =========================================================================

void agregarFrameXolo(
    float x, float y, float z, float rot,
    float delIzq, float delDer, float trasIzq, float trasDer,
    float cabeza, float cuello, float cola, float cuerpoY, float cuerpoRot)
{
    if (XoloFrameIndex >= XOLO_MAX_FRAMES) return;

    XOLO_FRAME& f = XoloFrames[XoloFrameIndex++];
    f.x = x; f.y = y; f.z = z; f.rotY = rot;
    f.pataDelIzq = delIzq; f.pataDelDer = delDer;
    f.pataTrasIzq = trasIzq; f.pataTrasDer = trasDer;
    f.cabeza = cabeza; f.cuello = cuello; f.cola = cola;
    f.cuerpoY = cuerpoY; f.cuerpoRot = cuerpoRot;
}

void interpolarXolo()
{
    if (XoloFrameIndex < 2) {
        XoloPlay = false;
        return;
    }

    int siguiente = (XoloPlayIndex + 1) % XoloFrameIndex;
    const XOLO_FRAME& a = XoloFrames[XoloPlayIndex];
    const XOLO_FRAME& b = XoloFrames[siguiente];

    float dx = b.x - a.x;
    float dz = b.z - a.z;
    float distancia = sqrtf(dx * dx + dz * dz);

    // Caminata tranquila: aproximadamente 0.7 unidades por segundo a 60 FPS.
    // Esto evita que el Xolo recorra cada tramo demasiado deprisa.
    XoloMaxSteps = (int)(distancia * 85.0f);
    if (XoloMaxSteps < 90) XoloMaxSteps = 90;
    XoloSteps = 0;
}

void inicializarXolo()
{
    // El Xolo inicia cerca del mariachi, ligeramente detras de el.
    // objRotY define la direccion hacia delante: (sin(yaw), cos(yaw)).
    XoloTrailCount = 0;
    XoloGaitTime = 0.0f;
    XoloSitBlend = 1.0f;
    XoloTrailReady = true;

    float yaw = glm::radians(objRotY);
    glm::vec3 frente(sinf(yaw), 0.0f, cosf(yaw));
    // El spawn actual del proyecto esta sobre el Muelle Rojo (altura 10.8).
    // DoMovement actualiza objPosY al primer ciclo, pero el Xolo se inicializa
    // antes; por eso tomamos aqui la misma altura del rectangulo de ese muelle.
    float alturaInicial = objPosY;
    if (objPosX >= -154.0f && objPosX <= -121.0f &&
        objPosZ >= 7.0f && objPosZ <= 10.5f) {
        alturaInicial = 10.8f;
    }
    glm::vec3 jugador(objPosX, alturaInicial - 5.7f, objPosZ);

    // No colocamos al Xolo a una distancia inventada detras del jugador:
    // ese punto podria quedar fuera del muelle o sobre el agua. Inicia en
    // una posicion segura del propio personaje y comienza a seguir el
    // historial real cuando el jugador ya haya recorrido suficiente camino.
    glm::vec3 inicioXolo = jugador;
    if (objPosX >= -154.0f && objPosX <= -121.0f &&
        objPosZ >= 7.0f && objPosZ <= 10.5f) {
        // En el spawn actual (Muelle Rojo), lo colocamos a un lado,
        // dentro del mismo rectangulo transitable.
        float candidatoX = objPosX + 3.0f;
        if (candidatoX <= -121.5f) inicioXolo.x = candidatoX;
    }

    XoloUltimaPosJugador = jugador;
    XoloHayPosJugadorAnterior = true;

    // La trayectoria inicial solo contiene puntos del muelle/suelo conocido.
    XoloTrail[XoloTrailCount++].posicion = inicioXolo;
    XoloTrail[XoloTrailCount++].posicion = jugador;

    xoloX = inicioXolo.x;
    xoloY = inicioXolo.y; // Altura del suelo: mismo desplazamiento vertical que el modelo del mariachi.
    xoloZ = inicioXolo.z;
    xoloRotY = objRotY;

    xoloPataDelIzq = 0.0f;
    xoloPataDelDer = 0.0f;
    xoloPataTrasIzq = 0.0f;
    xoloPataTrasDer = 0.0f;
    xoloCabeza = 0.0f;
    xoloCuello = 0.0f;
    xoloCola = 0.0f;
    xoloCuerpoY = 0.0f;
    xoloCuerpoRot = 0.0f;

    XoloPlay = true;
}

// Busca sobre la trayectoria un punto situado cierta distancia detras
// del jugador. Asi el perro recorre los mismos giros y puentes que el usuario.
static glm::vec3 obtenerPuntoSeguimientoXolo(float distancia, float& rotacion)
{
    if (XoloTrailCount <= 0) {
        rotacion = objRotY;
        return glm::vec3(objPosX, objPosY - 6.2f, objPosZ);
    }

    glm::vec3 actual = XoloTrail[XoloTrailCount - 1].posicion;
    float restante = distancia;

    for (int i = XoloTrailCount - 1; i > 0; --i) {
        glm::vec3 anterior = XoloTrail[i - 1].posicion;
        glm::vec3 tramo = actual - anterior;
        float longitud = glm::length(glm::vec3(tramo.x, 0.0f, tramo.z));

        if (longitud > 0.0001f) {
            if (longitud >= restante) {
                glm::vec3 direccion = glm::normalize(glm::vec3(tramo.x, 0.0f, tramo.z));
                glm::vec3 objetivo = actual - direccion * restante;
                objetivo.y = anterior.y + (actual.y - anterior.y) *
                    (1.0f - restante / longitud);
                rotacion = glm::degrees(atan2f(direccion.x, direccion.z));
                return objetivo;
            }
            restante -= longitud;
            actual = anterior;
        }
        else {
            actual = anterior;
        }
    }

    // Si aun no existe suficiente historial, usamos el punto mas antiguo.
    rotacion = objRotY;
    return XoloTrail[0].posicion;
}

void animarXolo()
{
    if (!XoloTrailReady) return;

    // La posicion vertical conserva el ajuste que ya funcionaba para el Xolo.
    glm::vec3 jugador(objPosX, objPosY - 5.7f, objPosZ);
    float movimientoJugador = 0.0f;
    if (XoloHayPosJugadorAnterior) {
        glm::vec3 cambio = jugador - XoloUltimaPosJugador;
        movimientoJugador = glm::length(glm::vec3(cambio.x, 0.0f, cambio.z));
    }
    XoloUltimaPosJugador = jugador;
    XoloHayPosJugadorAnterior = true;

    bool modoSeguir = (tipoCamara == 1 || tipoCamara == 2);
    bool jugadorCaminando = movimientoJugador > 0.002f;
    bool xoloAvanzando = false;

    if (modoSeguir) {
        // Registrar solamente posiciones ya validadas por el movimiento y colisiones
        // originales del mariachi.
        glm::vec3 ultimo = XoloTrail[XoloTrailCount - 1].posicion;
        glm::vec3 diferencia = jugador - ultimo;
        float distanciaNueva = glm::length(glm::vec3(diferencia.x, 0.0f, diferencia.z));

        if (distanciaNueva >= 0.04f) {
            if (XoloTrailCount < XOLO_MAX_TRAIL) {
                XoloTrail[XoloTrailCount++].posicion = jugador;
            }
            else {
                for (int i = 1; i < XOLO_MAX_TRAIL; ++i)
                    XoloTrail[i - 1] = XoloTrail[i];
                XoloTrail[XOLO_MAX_TRAIL - 1].posicion = jugador;
            }
        }

        float rotObjetivo = objRotY;
        glm::vec3 objetivo = obtenerPuntoSeguimientoXolo(XoloDistanciaSeguimiento, rotObjetivo);
        glm::vec3 posicionActual(xoloX, xoloY, xoloZ);
        glm::vec3 diferenciaObjetivo = objetivo - posicionActual;
        float distanciaObjetivo = glm::length(diferenciaObjetivo);

        // Seguir la trayectoria suavemente, sin teletransportarse ni cortar esquinas.
        if (distanciaObjetivo > 0.001f) {
            float pasoMaximo = 18.0f * deltaTime;
            float avance = (distanciaObjetivo < pasoMaximo) ? 1.0f : (pasoMaximo / distanciaObjetivo);
            posicionActual += diferenciaObjetivo * avance;
            xoloX = posicionActual.x;
            xoloZ = posicionActual.z;
        }

        // Seguir la altura registrada del mariachi.
        xoloY = objetivo.y;

        float diferenciaRot = rotObjetivo - xoloRotY;
        while (diferenciaRot > 180.0f) diferenciaRot -= 360.0f;
        while (diferenciaRot < -180.0f) diferenciaRot += 360.0f;
        xoloRotY += diferenciaRot * (1.0f - expf(-5.0f * deltaTime));

        xoloAvanzando = distanciaObjetivo > 0.08f;
    }

    // Si el jugador cambia a cámara libre, el Xolo deja de seguirlo.
    // No se sienta: simplemente termina el paso y se queda quieto.
    bool caminando = modoSeguir && (jugadorCaminando || xoloAvanzando);

    // Reproducir el sonido una sola vez cuando el Xolo comienza a caminar
    static bool perroEstabaCaminando = false;

    if (caminando && !perroEstabaCaminando) {
        mciSendStringA("seek sonidoPerro to start", NULL, 0, NULL);
        mciSendStringA("play sonidoPerro", NULL, 0, NULL);
    }

    perroEstabaCaminando = caminando;

    // Cada fila representa una pose completa del Xolo.
    // Orden: patas delantera izq./der., trasera izq./der., cabeza,
    // cuello, cola, elevacion del cuerpo y rotacion del cuerpo.
    // Toda la animacion articulada y el rebote corporal se definen en estos keyframes.
    static const float posesCaminar[8][9] = {
        {  27.0f, -10.0f, -10.0f,  27.0f, -15.0f,  10.0f, -32.0f, 0.018f,  2.4f },
        {  19.0f,  -3.0f,  -3.0f,  19.0f,  -8.0f,   5.0f, -14.0f, 0.014f,  1.7f },
        {   5.0f,  12.0f,  12.0f,   5.0f,  12.0f,  -6.0f,  20.0f, 0.006f,  0.5f },
        { -16.0f,   7.0f,   7.0f, -16.0f,  18.0f, -10.0f,  34.0f, 0.002f, -1.5f },
        { -27.0f,  10.0f,  10.0f, -27.0f,  15.0f, -10.0f,  32.0f, 0.018f, -2.4f },
        { -19.0f,   3.0f,   3.0f, -19.0f,   8.0f,  -5.0f,  14.0f, 0.014f, -1.7f },
        {  -5.0f, -12.0f, -12.0f,  -5.0f, -12.0f,   6.0f, -20.0f, 0.006f, -0.5f },
        {  16.0f,  -7.0f,  -7.0f,  16.0f, -18.0f,  10.0f, -34.0f, 0.002f,  1.5f }
    };

    // La transicion entre caminar y reposo tambien interpola hacia una pose neutral.
    // De esta forma no se agregan oscilaciones matematicas independientes.
    static float pesoCaminata = 0.0f;
    const float duracionTransicion = 0.18f;
    if (caminando) {
        XoloGaitTime += deltaTime;
        pesoCaminata += deltaTime / duracionTransicion;
        if (pesoCaminata > 1.0f) pesoCaminata = 1.0f;
    }
    else {
        pesoCaminata -= deltaTime / duracionTransicion;
        if (pesoCaminata < 0.0f) pesoCaminata = 0.0f;
    }

    // Ciclo corto para que el Xolo pueda seguir el ritmo del mariachi.
    const float duracionCiclo = 0.98f;
    float faseCompleta = fmodf(XoloGaitTime, duracionCiclo) /
        duracionCiclo * 8.0f;
    int fase = (int)faseCompleta;
    int siguiente = (fase + 1) % 8;
    float t = faseCompleta - (float)fase;

    // Interpolacion smoothstep entre las poses clave.
    float suave = t * t * (3.0f - 2.0f * t);
    float pesoSuave = pesoCaminata * pesoCaminata * (3.0f - 2.0f * pesoCaminata);

    float pose[9];
    for (int i = 0; i < 9; ++i) {
        float valorInterpolado = posesCaminar[fase][i] +
            (posesCaminar[siguiente][i] - posesCaminar[fase][i]) * suave;
        // La pose neutral es cero; pesoSuave mezcla entre reposo y caminata.
        pose[i] = valorInterpolado * pesoSuave;
    }

    xoloPataDelIzq = pose[0];
    xoloPataDelDer = pose[1];
    xoloPataTrasIzq = pose[2];
    xoloPataTrasDer = pose[3];
    xoloCabeza = pose[4];
    xoloCuello = pose[5];
    xoloCola = pose[6];
    xoloCuerpoY = pose[7];
    xoloCuerpoRot = pose[8];
}

// Aplica una rotacion alrededor de un pivote local sin desplazar la pieza.
static glm::mat4 xoloRotarPivote(const glm::mat4& base,
    const glm::vec3& pivote, float angulo, const glm::vec3& eje)
{
    glm::mat4 m = glm::translate(base, pivote);
    m = glm::rotate(m, glm::radians(angulo), eje);
    m = glm::translate(m, -pivote);
    return m;
}

void dibujarXolo(Shader& shader, GLint modelLoc,
    Model& tronco, Model& cuello, Model& cabeza, Model& cola,
    Model& pataDelIzq, Model& pataDelDer,
    Model& pataTrasIzq, Model& pataTrasDer)
{
    glm::mat4 raiz(1.0f);
    // Pequeño ajuste visual de altura, sin modificar la trayectoria de seguimiento.
    raiz = glm::translate(raiz, glm::vec3(xoloX, xoloY + 0.10f, xoloZ));
    raiz = glm::rotate(raiz, glm::radians(xoloRotY), glm::vec3(0, 1, 0));
    raiz = glm::scale(raiz, glm::vec3(3.0f));

    glm::mat4 cuerpo = glm::translate(raiz, glm::vec3(0.0f, xoloCuerpoY, 0.0f));
    cuerpo = xoloRotarPivote(cuerpo, glm::vec3(0.0f, 0.84f, 0.14f),
        xoloCuerpoRot, glm::vec3(1, 0, 0));

    glUniform1i(glGetUniformLocation(shader.Program, "transparency"), 0);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cuerpo));
    tronco.Draw(shader);

    glm::mat4 mCuello = xoloRotarPivote(cuerpo,
        glm::vec3(0.0f, 1.08f, 0.56f), xoloCuello, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mCuello));
    cuello.Draw(shader);

    glm::mat4 mCabeza = xoloRotarPivote(mCuello,
        glm::vec3(0.0f, 1.30f, 0.76f), xoloCabeza, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mCabeza));
    cabeza.Draw(shader);

    glm::mat4 mCola = xoloRotarPivote(cuerpo,
        glm::vec3(0.0f, 0.93f, -0.24f), xoloCola, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mCola));
    cola.Draw(shader);

    // Pivotes aproximados en las articulaciones de las patas (coordenadas del OBJ).
    glm::mat4 mPata = xoloRotarPivote(cuerpo,
        glm::vec3(0.1359f, 0.82f, 0.477f), xoloPataDelIzq, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mPata));
    pataDelIzq.Draw(shader);

    mPata = xoloRotarPivote(cuerpo,
        glm::vec3(-0.1187f, 0.82f, 0.484f), xoloPataDelDer, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mPata));
    pataDelDer.Draw(shader);

    mPata = xoloRotarPivote(cuerpo,
        glm::vec3(0.1064f, 0.82f, -0.355f), xoloPataTrasIzq, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mPata));
    pataTrasIzq.Draw(shader);

    mPata = xoloRotarPivote(cuerpo,
        glm::vec3(-0.1255f, 0.82f, -0.333f), xoloPataTrasDer, glm::vec3(1, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mPata));
    pataTrasDer.Draw(shader);
}
