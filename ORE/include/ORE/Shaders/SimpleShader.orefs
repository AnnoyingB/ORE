#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int id;
  
in vec2 TexCoord;
in vec4 vertexColor;

uniform sampler2D tex;
uniform bool hasTex = false;
uniform int primitiveID;

void main()
{
    if(hasTex)
    {
        vec4 texColor = texture(tex, TexCoord);
        if(texColor.a < 0.1)
            discard;
        FragColor = texColor * vertexColor;
    } else {
        FragColor = vertexColor;
    }

    id = primitiveID;
} 