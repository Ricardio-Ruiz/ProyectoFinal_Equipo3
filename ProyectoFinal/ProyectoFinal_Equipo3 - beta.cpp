// cosas pa hacer:
// 
// iluminación avanzada (1 Dir, 7 Point, 2 Spot),
// camara tipo FPS (mundo abierto), y 3 tipos de animación (Keyframes, Procedimental, Estados).
//
//BORRAAARRRR /\ /\ /\ /\  /\ /\ /\ /\ /\ /\ /\


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

// Prototipos
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void Animation();
void saveFramesToFile();
void loadFramesFromFile();
void resetElements();
void interpolation();

// Dimensiones y Cámara
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;
Camera camera(glm::vec3(-120.0f, 16.0f, 10.0f)); // Posición inicial
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
    glm::vec3( 2.0f,  2.0f,  2.0f),
    glm::vec3(-2.0f,  2.0f, -2.0f),
    glm::vec3( 4.0f,  1.5f,  0.0f),
    glm::vec3(-4.0f,  1.5f,  0.0f),
    glm::vec3( 0.0f,  3.0f,  4.0f),
    glm::vec3( 0.0f,  3.0f, -4.0f),
    glm::vec3( 5.0f,  2.0f,  5.0f)
};

glm::vec3 spotLightPositions[2] = {
    glm::vec3( 0.0f, 5.0f, 0.0f),  // Ej. Reflector 1 fijo
    glm::vec3( 0.0f, 0.0f, 0.0f)   // Ej. Linterna del jugador (se actualiza en el loop)
};

glm::vec3 spotLightDirections[2] = {
    glm::vec3( 0.0f, -1.0f, 0.0f), // Apuntando hacia abajo
    glm::vec3( 0.0f,  0.0f, -1.0f) // Apuntando hacia donde mira la cámara
};

// =========================================================================
// 2. VARIABLES DE ANIMACIÓN (3 Tipos)
// =========================================================================

// A) Procedimental (Basada en matemáticas/tiempo)
float waterOffset = 0.0f;
float floatingY = 0.0f;

// B) Máquina de Estados (Ej. Ciclo de día/noche o compuertas)
int estadoDiaNoche = 0; // 0=Día, 1=Tarde, 2=Noche
bool activarLinterna = true;

// C) Keyframes (Estructura genérica para personaje principal o un objeto complejo)
#define MAX_FRAMES 200
int i_max_steps = 90;
int i_curr_steps = 0;

float objPosX = 0.0f, objPosY = 0.0f, objPosZ = 0.0f;
float objRotY = 0.0f, art1 = 0.0f, art2 = 0.0f; // articulaciones genéricas

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

// =========================================================================
// MAIN
// =========================================================================
int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final - Chinampas y Trajineras", nullptr, nullptr);
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

    // =========================================================================
    // 3. CARGA DE MODELOS (Espacio para assets del proyecto)
    // =========================================================================
    Model ChinampaA((char*)"Models/ZonaA/ChinampaA.obj");
    Model ChinampaB((char*)"Models/ZonaB/ChinampaB.obj");
    Model ChinampaC((char*)"Models/ZonaC/ChinampaC.obj");
    Model ChinampaD((char*)"Models/ZonaD/ChinampaD.obj");
    Model ChinampaE((char*)"Models/ZonaE/ChinampaE.obj");
    Model Chinampas((char*)"Models/Extras-Relleno/Chinampas.obj");
    
    //Model Trajinera((char*)"Models/Proyecto/Trajinera.obj");

    //Model Personaje((char*)"Models/Proyecto/Personaje.");



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


    
    // Inicializar Keyframes
    resetElements();

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();
        Animation();

        // Color de fondo (Cambia según estadoDiaNoche)
        //if(estadoDiaNoche == 0) glClearColor(0.5f, 0.8f, 0.9f, 1.0f);      // Día
        //else if(estadoDiaNoche == 1) glClearColor(0.8f, 0.4f, 0.2f, 1.0f); // Tarde
        //else glClearColor(0.05f, 0.05f, 0.1f, 1.0f);                       // Noche
        glClearColor(0.6588f, 0.4431f, 0.0392f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Use();
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // =========================================================================
        // PASO DE UNIFORMES DE ILUMINACIÓN
        // =========================================================================
        // Luz Direccional (Sol/Luna)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

        // 7 Luces Puntuales (Antorchas, faroles en trajineras, etc.)
        for(int i = 0; i < 7; i++) {
            std::string number = std::to_string(i);
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].position").c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
            
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].ambient").c_str()), 0.2f, 0.1f, 0.05f);
            
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].diffuse").c_str()), 0.8f, 0.4f, 0.1f);
            
            glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].specular").c_str()), 1.0f, 0.5f, 0.2f);
            
            glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].constant").c_str()), 1.0f);
            
            glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].linear").c_str()), 0.09f);
            
            glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + number + "].quadratic").c_str()), 0.032f);
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
        glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);        //ver mas
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // =========================================================================
        // DIBUJADO DE MODELOS CON SUS MATRICES
        // =========================================================================
        glm::mat4 model(1.0f);
        glm::mat4 modelTemp(1.0f);
        
        // 1. Entorno Estático (Chinampas)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        Chinampas.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaA.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaB.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaC.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaD.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        ChinampaE.Draw(lightingShader);

        //// 2. Animación Procedimental (Agua moviéndose)
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(waterOffset, 0.0f, waterOffset)); 
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1); // Agua transparente
        //PisoAgua.Draw(lightingShader);

        //// 3. Animación Procedimental (Trajinera flotando)
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(5.0f, floatingY, -5.0f));
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

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

// =========================================================================
// LÓGICA DE ACTUALIZACIÓN DE ANIMACIONES
// =========================================================================
void Animation() {
    // 1. Animaciones Procedimentales
    waterOffset = sin(glfwGetTime() * 0.5f) * 0.2f;
    floatingY = sin(glfwGetTime() * 2.0f) * 0.1f;

    // 2. Animación Keyframes
    if (playAnim) {
        if (i_curr_steps >= i_max_steps) {
            playIndex++;
            if (playIndex > FrameIndex - 2) {
                playIndex = 0;
                playAnim = false;
            } else {
                i_curr_steps = 0;
                interpolation();
            }
        } else {
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
void DoMovement() {
    // Cámara WASD
    if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);

    // Grabación Keyframes (Controles para mover el modelo genérico)
    if (keys[GLFW_KEY_UP]) objPosZ -= 2.0f * deltaTime;
    if (keys[GLFW_KEY_DOWN]) objPosZ += 2.0f * deltaTime;
    if (keys[GLFW_KEY_LEFT]) objPosX -= 2.0f * deltaTime;
    if (keys[GLFW_KEY_RIGHT]) objPosX += 2.0f * deltaTime;
    if (keys[GLFW_KEY_U]) art1 += 50.0f * deltaTime;
    if (keys[GLFW_KEY_J]) art1 -= 50.0f * deltaTime;

    // Archivos de Animación
    if (keys[GLFW_KEY_F1]) saveFramesToFile();
    if (keys[GLFW_KEY_F2]) loadFramesFromFile();
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
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
        } else {
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

}

void MouseCallback(GLFWwindow *window, double xPos, double yPos) {
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
    if(FrameIndex > 0) {
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