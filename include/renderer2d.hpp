#pragma once
#include "pipeline.hpp"
#include "sprite.hpp"

// manages 2D rendering state with orthographic projection
struct Renderer2D {
    void init() {
        _pipeline.init("sprite.vert", "sprite.frag");
        _sprite.init();
    }
    void destroy() {
        _sprite.destroy();
        _pipeline.destroy();
    }

    // begin 2D rendering pass
    void begin(float screen_width = 1280.0f, float screen_height = 720.0f) {
        _pipeline.bind();

        // set up orthographic projection (top-left origin)
        glm::mat4x4 projection = glm::ortho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(12, 1, false, glm::value_ptr(projection));

        // identity view matrix (no camera transform needed for 2D)
        glm::mat4x4 identity(1.0f);
        glUniformMatrix4fv(8, 1, false, glm::value_ptr(identity));

        // disable 3D-specific states
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // enable alpha blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // end 2D rendering pass, restore 3D state
    void end() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }

    // convenience methods that use the shared sprite quad
    void draw_solid(float x, float y, float w, float h, glm::vec4 color) {
        _sprite.draw_solid(x, y, w, h, color);
    }
    void draw_textured(Texture& texture, float x, float y, float w, float h, glm::vec4 tint = glm::vec4(1.0f)) {
        _sprite.draw_textured(texture, x, y, w, h, tint);
    }
    void draw_sub(Texture& texture, float x, float y, float w, float h,
                  float u0, float v0, float u1, float v1, glm::vec4 tint = glm::vec4(1.0f)) {
        _sprite.draw_sub(texture, x, y, w, h, u0, v0, u1, v1, tint);
    }

    Pipeline _pipeline;
    Sprite _sprite;
};
