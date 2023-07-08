#version 450 core
out vec4 FragColor;

in vec3 localPos;
  
uniform samplerCube environmentMap;
uniform float exposure = 1.0;
uniform float gamma = 2.2;

void main()
{
    vec3 envColor = texture(environmentMap, localPos).rgb;
    
    // reinhard tone mapping
    envColor = vec3(1.0) - exp(-envColor * exposure);
    // gamma correction
    envColor = pow(envColor, vec3(1.0/gamma)); 

    FragColor.rgb = pow(envColor.rgb, vec3(1.0/gamma));
    //FragColor = vec4(envColor, 1.0);
}