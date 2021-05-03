#version 420 core                                                           
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec2 in_texcoords;

layout(location=0) out vec3 normal;
layout(location=1) out vec2 texcoords;

uniform mat4 View;                                                          
uniform mat4 Projection;

void main()                                                                 
{
    normal = in_normal;
    texcoords = in_texcoords;
    gl_Position = Projection * View * vec4(in_position, 1.0);
}                                                                           
