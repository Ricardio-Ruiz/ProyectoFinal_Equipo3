#version 330 core
layout (location = 0) in vec3 position;

//Añadiendole la entrada de las coordenadas de la textura para pasarlas al fragment shade
layout (location = 2) in vec2 texCoords; //le recibe las coordenadas de textura del modelooo

out vec2 TexCoords;                     //variable de salida para el fragment shaderrr

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    TexCoords = texCoords;              //se le pasa las coordenadas
    
}