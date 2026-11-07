#version 330 core

#define NUMBER_OF_POINT_LIGHTS 7        //cant de luces
#define NUMBER_OF_SPOT_LIGHTS 2

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;    //brillo
};

struct DirLight             //Direccional
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight           //Puntual
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight            //Reflector
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS];
//uniform SpotLight spotLight;                                                        //no
uniform SpotLight spotLights[NUMBER_OF_SPOT_LIGHTS];                               //<--- si
uniform Material material;
uniform int transparency;       //Para reconocer las texturas con transparencia

uniform float uvOffset;     //anim awa



// Function prototypes
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir );
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir );

void main( )
{
    // Properties
    vec3 norm = normalize( Normal );
    vec3 viewDir = normalize( viewPos - FragPos );
    
    // Directional lighting
    vec3 result = CalcDirLight( dirLight, norm, viewDir );
    
    // Point lights
    for ( int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++ )
    {
        result += CalcPointLight( pointLights[i], norm, FragPos, viewDir );
    }
    
    // Spot lights
    for ( int i = 0; i < NUMBER_OF_SPOT_LIGHTS; i++ )
    {
        result += CalcSpotLight( spotLights[i], norm, FragPos, viewDir );
    }

    //        //combinar ahora si los resultados 	
    //        //color = vec4( result,texture(material.diffuse, TexCoords).rgb ); 
    //        color = vec4( result, texture(material.diffuse, TexCoords).a );         //combinar con canal alfa de la textura difusa
	//          if(color.a < 0.1 && transparency==1)      //activsción del canal Alfa para la transparencia
    //            discard;

    // 1. Extraemos el canal Alpha de la textura difusa
    float alphaVal = texture(material.diffuse, TexCoords).a;
    
    // 2. Si no estamos usando texturas con transparencia, forzamos el alfa a 1.0 (opaco)
    if (transparency == 0) {
        alphaVal = 1.0f;
    }
    
    // 3. Aplicamos el color final con la luz calculada
    color = vec4(result, alphaVal);
    
    // 4. Descartar solo si es una textura transparente y el alfa es casi invisible
    if (alphaVal < 0.1 && transparency == 1) {
        discard;
    }

}

// Calculates the color when using a directional light. DIRECCIONAL--------------------------------
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir )
{
    vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords + vec2(uvOffset, uvOffset * 0.9)) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords+ vec2(uvOffset, uvOffset * 0.9) ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords + vec2(uvOffset, uvOffset * 0.9) ) );
    
    return ( ambient + diffuse + specular );
}

// Calculates the color when using a point light. PUNTUAL -----------------------------------------
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords + vec2(uvOffset, uvOffset * 0.9)) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords + vec2(uvOffset, uvOffset * 0.9)) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords + vec2(uvOffset, uvOffset * 0.9) ) );
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ( ambient + diffuse + specular );
}

// Calculates the color when using a spot light. REFLECTOR ----------------------------------------
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords + vec2(uvOffset, uvOffset * 0.9)) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords + vec2(uvOffset, uvOffset * 0.9)) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords + vec2(uvOffset, uvOffset * 0.9) ) );
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return ( ambient + diffuse + specular );
}