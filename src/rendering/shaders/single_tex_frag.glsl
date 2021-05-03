#version 420 core                                                           

layout(location=0) in vec3 normal;
layout(location=1) in vec2 texcoords;

uniform sampler2D diffuse_texture;

out vec4 fragColor;

void main()                                                                 
{
    const vec3 lightDir = vec3(-0.5, 0.8, 0);
    const float ambient = 0.2;
    
    float diff = max(dot(normal, lightDir), 0.0);
    float intensity = min(diff + ambient, 1.0);

    vec4 tex = texture(diffuse_texture, texcoords);
    
    fragColor = vec4(intensity * tex.rgb, 1.0);                               
}                                                                          
