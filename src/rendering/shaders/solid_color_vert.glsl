#version 330 core                                                           
layout(location=0) in vec3 in_Position;                                     
uniform mat4 View;                                                          
uniform mat4 Projection;                                                    
void main()                                                                 
{                                                                           
    gl_Position = Projection * View * vec4(in_Position, 1.0);               
}                                                                           
