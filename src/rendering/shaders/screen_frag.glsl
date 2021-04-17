#version 330 core
out vec4 frag_color;
in vec2 tex_coords;
uniform sampler2D screen_texture;

void main() {
    vec4 tex = texture(screen_texture, tex_coords);
    frag_color = vec4(tex.rgb, 1.0);
}
