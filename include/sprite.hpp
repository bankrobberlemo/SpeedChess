#pragma once
#include "mesh.hpp"
#include "texture.hpp"

// a simple 2D quad for rendering sprites and UI elements
struct Sprite {
    void init() {
        // unit quad from (0,0) to (1,1)
        std::vector<Vertex> vertices = {
            Vertex{ glm::vec3{0, 0, 0}, glm::vec3{0, 0, 1}, glm::vec2{0, 0}, glm::vec4{1, 1, 1, 1} },
            Vertex{ glm::vec3{1, 0, 0}, glm::vec3{0, 0, 1}, glm::vec2{1, 0}, glm::vec4{1, 1, 1, 1} },
            Vertex{ glm::vec3{0, 1, 0}, glm::vec3{0, 0, 1}, glm::vec2{0, 1}, glm::vec4{1, 1, 1, 1} },
            Vertex{ glm::vec3{1, 1, 0}, glm::vec3{0, 0, 1}, glm::vec2{1, 1}, glm::vec4{1, 1, 1, 1} },
        };
        std::vector<Index> indices = { 0, 1, 3, 3, 2, 0 };
        _mesh.init_raw(vertices, indices);
    }
    void destroy() {
        _mesh.destroy();
    }

    // draw at position (x,y) with size (w,h) and optional tint
    void draw(float x, float y, float w, float h, glm::vec4 tint = glm::vec4(1.0f)) {
        glm::mat4x4 model(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));
        glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
        glUniform4fv(40, 1, glm::value_ptr(tint));

        // reset UV to full texture by default
        glUniform2f(42, 0.0f, 0.0f);
        glUniform2f(43, 1.0f, 1.0f);

        _mesh.bind();
        _mesh.draw();
    }

    // draw with a texture
    void draw_textured(Texture& texture, float x, float y, float w, float h, glm::vec4 tint = glm::vec4(1.0f)) {
        glUniform1f(41, 1.0f);
        texture.bind();
        draw(x, y, w, h, tint);
    }

    // draw a sub-region of a texture (for font atlas)
    void draw_sub(Texture& texture, float x, float y, float w, float h,
                  float u0, float v0, float u1, float v1, glm::vec4 tint = glm::vec4(1.0f)) {
        glUniform1f(41, 1.0f);
        texture.bind();

        glm::mat4x4 model(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));
        glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
        glUniform4fv(40, 1, glm::value_ptr(tint));

        // set UV sub-region
        glUniform2f(42, u0, v0);
        glUniform2f(43, u1 - u0, v1 - v0);

        _mesh.bind();
        _mesh.draw();
    }

    // draw a solid colored quad (no texture)
    void draw_solid(float x, float y, float w, float h, glm::vec4 color) {
        glUniform1f(41, 0.0f);
        draw(x, y, w, h, color);
    }

    Mesh _mesh;
};
