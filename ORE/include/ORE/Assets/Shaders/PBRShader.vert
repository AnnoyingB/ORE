#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec3 aNorm;

//out vec4 vertexColor;
out vec3 Normal;
out vec3 WorldPos;
out vec4 VertexCol;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    WorldPos = vec3(model * vec4(aPos, 1.0));
    VertexCol = aCol;
    Normal = normalMatrix * aNorm;
}