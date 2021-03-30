#version 330 core                                                           
in vec3 color;
in vec3 normal;

out vec4 fragColor;

void main()                                                                 
{
    const vec3 lightDir = vec3(-0.5, 0.8, 0);
    const float ambient = 0.2;
    
    float diff = max(dot(normal, lightDir), 0.0);
    float intensity = min(diff + ambient, 1.0);
    
    fragColor = vec4(intensity * color, 1.0);                               
}                                                                          
