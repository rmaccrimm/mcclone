#version 330 core                                                           
in vec3 color;
in vec3 normal;

void main()                                                                 
{
    vec3 lightDir = vec3(-0.5, 0.8, 0);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;
    gl_FragColor = vec4(diffuse + 0.1 * color, 1.0);                               
}                                                                          
