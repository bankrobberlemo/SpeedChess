#pragma once
#include "../renderer2d.hpp"
#include "board.hpp"

struct BoardRenderer {
    static constexpr float SQUARE_SIZE = 80.0f;
    static constexpr float BOARD_SIZE = SQUARE_SIZE * Board::SIZE; // 640px

    float origin_x = 320.0f;
    float origin_y = 40.0f;

    void update_origin(float screen_w, float screen_h) {
        origin_x = (screen_w - BOARD_SIZE) / 2.0f;
        origin_y = 40.0f;
    }

    static constexpr glm::vec4 LIGHT_SQUARE = { 0.93f, 0.84f, 0.69f, 1.0f };
    static constexpr glm::vec4 DARK_SQUARE  = { 0.70f, 0.53f, 0.33f, 1.0f };

    auto board_to_screen(int col, int row) const -> std::pair<float, float> {
        return {
            origin_x + col * SQUARE_SIZE,
            origin_y + row * SQUARE_SIZE
        };
    }

    void draw(Renderer2D& renderer) {
        for (int row = 0; row < Board::SIZE; row++) {
            for (int col = 0; col < Board::SIZE; col++) {
                auto [x, y] = board_to_screen(col, row);
                bool is_light = (col + row) % 2 == 0;
                renderer.draw_solid(x, y, SQUARE_SIZE, SQUARE_SIZE,
                    is_light ? LIGHT_SQUARE : DARK_SQUARE);
            }
        }
    }

    void draw_highlight(Renderer2D& renderer, int col, int row, glm::vec4 color) {
        if (!Board::in_bounds(col, row)) return;
        auto [x, y] = board_to_screen(col, row);
        renderer.draw_solid(x, y, SQUARE_SIZE, SQUARE_SIZE, color);
    }

    void draw_border(Renderer2D& renderer, int col, int row, glm::vec4 color, float thickness = 4.0f) {
        if (!Board::in_bounds(col, row)) return;
        auto [x, y] = board_to_screen(col, row);
        float s = SQUARE_SIZE;
        renderer.draw_solid(x, y, s, thickness, color);
        renderer.draw_solid(x, y + s - thickness, s, thickness, color);
        renderer.draw_solid(x, y, thickness, s, color);
        renderer.draw_solid(x + s - thickness, y, thickness, s, color);
    }

    void draw_dot(Renderer2D& renderer, int col, int row, glm::vec4 color, float radius = 10.0f) {
        if (!Board::in_bounds(col, row)) return;
        auto [x, y] = board_to_screen(col, row);
        float cx = x + SQUARE_SIZE / 2.0f - radius;
        float cy = y + SQUARE_SIZE / 2.0f - radius;
        renderer.draw_solid(cx, cy, radius * 2.0f, radius * 2.0f, color);
    }
};
