#pragma once
#include "../renderer2d.hpp"

struct BitmapFont {
    void init(const std::string& atlas_path) {
        _atlas.init(atlas_path);
        _atlas.set_nearest_filter();
    }
    void destroy() {
        _atlas.destroy();
    }

    void draw_text(Renderer2D& renderer, const std::string& text, float x, float y,
                   float scale = 2.0f, glm::vec4 color = glm::vec4(1.0f)) {
        float cursor_x = x;

        for (char ch : text) {
            if (ch < 32 || ch > 127) continue;

            int char_index = ch - 32;
            int atlas_col = char_index % _columns;
            int atlas_row = char_index / _columns;

            float u0 = static_cast<float>(atlas_col) * _glyph_u;
            float v0 = static_cast<float>(atlas_row) * _glyph_v;
            float u1 = u0 + _glyph_u;
            float v1 = v0 + _glyph_v;

            float char_w = _glyph_width * scale;
            float char_h = _glyph_height * scale;

            renderer.draw_sub(_atlas, cursor_x, y, char_w, char_h, u0, v0, u1, v1, color);
            cursor_x += char_w;
        }
    }

    float measure_width(const std::string& text, float scale = 2.0f) const {
        return text.length() * _glyph_width * scale;
    }

    float measure_height(float scale = 2.0f) const {
        return _glyph_height * scale;
    }

    Texture _atlas;

    // 16 columns, 6 rows in the atlas (ASCII 32-127)
    int _columns = 16;
    int _rows = 6;
    float _glyph_width = 8.0f;
    float _glyph_height = 16.0f;
    float _glyph_u = 1.0f / 16.0f;
    float _glyph_v = 1.0f / 6.0f;
};
