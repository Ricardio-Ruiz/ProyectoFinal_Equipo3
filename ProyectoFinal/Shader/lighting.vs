#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform float time;
uniform int isPapelPicado;


void main()
{
    vec3 posAnimada = position;
    
    // Si el interruptor esta activo, ondulamos los vrrtices del papel picao
    if (isPapelPicado == 1) {
        // Onda suave que recorre la tira
        posAnimada.x += sin(time * 3.0 + position.y * 2.0) * 0.15;
        posAnimada.z += cos(time * 2.5 + position.x * 2.0) * 0.15;
    }
    
    gl_Position = projection * view * model * vec4(posAnimada, 1.0f);
    FragPos = vec3(model * vec4(posAnimada, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;
}