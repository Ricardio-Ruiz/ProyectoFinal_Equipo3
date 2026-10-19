#version 330 core
out vec4 FragColor;
  
//Recibiendo las coordenadas y usando la textura que la clase Model ya carga automaticamnete
in vec2 TexCoords;                       //recibe desde el vertex shader

//clase Model de Assimp usa este nombre por defecto para la textura 
uniform sampler2D texture_diffuse1;

void main()
{
     FragColor = vec4(1.0f);

      //pintando el fragmento usando el color exacto de la textura
    FragColor = texture(texture_diffuse1, TexCoords);


    //Para darle ese efecto de que brilla la esfera
    //se obtiene  el color base de la textura
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    //ahora se multiplica el color para "sobreexponerlo" 
    float intensidad= 1.25;
    vec3 colorBrillante = texColor.rgb * intensidad;

    //Se puede sumar un ligero tono blanco constante si se quiere que resalte aún mas:
    // colorBrillante += vec3(0.15);

    FragColor = vec4(colorBrillante, texColor.a);           //mezcla final
}