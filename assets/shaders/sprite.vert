#version 460 core

// input (from vertex)
layout(location = 0) in vec3 in_pos;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_col;

// output (to fragment shader)
layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec4 out_col;

// transforms
layout(location =  0) uniform mat4x4 model_transform;
layout(location =  8) uniform mat4x4 view_transform;
layout(location = 12) uniform mat4x4 projection;

// UV offset and scale for sub-region rendering (font atlas, spritesheets)
layout(location = 42) uniform vec2 uv_offset = vec2(0.0, 0.0);
layout(location = 43) uniform vec2 uv_scale = vec2(1.0, 1.0);

void main() {
    gl_Position = projection * view_transform * model_transform * vec4(in_pos, 1.0);
    out_uv = uv_offset + in_uv * uv_scale;
    out_col = in_col;
}
