#version 330 core                                                           
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;
layout(location=2) in vec3 in_normal;

out vec3 color;
out vec3 normal;

uniform mat4 View;                                                          
uniform mat4 Projection;

void main()                                                                 
{
    color = in_color;
    normal = in_normal;
    gl_Position = Projection * View * vec4(in_position, 1.0);
}                                                                           
