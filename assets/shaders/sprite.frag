#version 460 core

// input (from vertex shader)
layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_col;

// output
layout(location = 0) out vec4 out_color;

// texture
layout(binding = 0) uniform sampler2D tex_sprite;

// custom uniforms (start at 40 to avoid conflicts with existing shaders)
layout(location = 40) uniform vec4 tint_color = vec4(1.0);
layout(location = 41) uniform float use_texture = 1.0;

void main() {
    vec4 tex_col = texture(tex_sprite, in_uv);
    vec4 base = mix(in_col, tex_col, use_texture);
    out_color = base * tint_color;

    // discard fully transparent pixels
    if (out_color.a < 0.01) discard;
}
