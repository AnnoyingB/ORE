#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int id;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

in vec4 vertexColor;
in vec3 aNormal;
in vec3 FragPos;

uniform int primitiveID;

uniform vec3 viewPos;

vec3 CalculateDirectionalLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(material.diffuse);
    vec3 diffuse  = light.diffuse  * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular);
    return (ambient + diffuse + specular);
}

void main()
{
    /*if(lightingType == 0) {
        vec3 result = material.ambient * lightColor;
        FragColor = vec4(result, 1.0);
    } else if(lightingType == 1) {
        // diffuse
        vec3 norm = normalize(aNormal);
        vec3 lightDir = normalize(lightPosition - FragPos);
        
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 result = lightColor * (diff * material.diffuse);
        FragColor = vec4(result, 1.0);
    } else if(lightingType == 2) {
        // phong
        // diffuse
        vec3 norm = normalize(aNormal);
        vec3 lightDir = normalize(lightPosition - FragPos);
        
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lightColor * (diff * material.diffuse);

        // ambient
        vec3 ambient = material.ambient * lightColor;

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lightColor * (spec * material.specular);

        // phong result
        vec3 result = (ambient + diffuse + specular) * vec3(vertexColor);
        FragColor = vec4(result, 1.0);
    }*/

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0);
    result += CalculateDirectionalLight(dirLight, normalize(aNormal), viewDir);
    //result *= vec3(vertexColor);
    FragColor = vec4(result, 1.0);

    id = primitiveID;
}