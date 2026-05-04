#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 Normal;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
}