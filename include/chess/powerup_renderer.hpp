#pragma once
#include "../renderer2d.hpp"
#include "board_renderer.hpp"
#include "powerup.hpp"

struct PowerUpRenderer {
    void init() {
        _tex_teleport.init("chess/powerup_teleport.png");
        _tex_teleport.set_nearest_filter();
        _tex_bomb.init("chess/powerup_bomb.png");
        _tex_bomb.set_nearest_filter();
        _tex_freeze.init("chess/powerup_freeze.png");
        _tex_freeze.set_nearest_filter();
        _tex_promotion.init("chess/powerup_promotion.png");
        _tex_promotion.set_nearest_filter();
    }

    void destroy() {
        _tex_teleport.destroy();
        _tex_bomb.destroy();
        _tex_freeze.destroy();
        _tex_promotion.destroy();
    }

    void draw(Renderer2D& renderer, const BoardRenderer& board_renderer,
              const PowerUpManager& manager, float time) {
        for (int i = 0; i < PowerUpManager::MAX_POWERUPS; i++) {
            const PowerUp& p = manager.get_powerups()[i];
            if (!p.active) continue;

            auto [sx, sy] = board_renderer.board_to_screen(p.col, p.row);

            // glow effect
            float pulse = 0.6f + 0.2f * std::sin(time * 3.0f);
            glm::vec4 glow_color = get_glow_color(p.type, pulse);
            float pad = 6.0f;
            renderer.draw_solid(sx + pad, sy + pad,
                BoardRenderer::SQUARE_SIZE - pad * 2, BoardRenderer::SQUARE_SIZE - pad * 2,
                glow_color);

            float icon_pad = 12.0f;
            float icon_size = BoardRenderer::SQUARE_SIZE - icon_pad * 2;
            Texture& tex = get_texture(p.type);
            renderer.draw_textured(tex, sx + icon_pad, sy + icon_pad, icon_size, icon_size);
        }
    }

private:
    glm::vec4 get_glow_color(PowerUpType type, float alpha) {
        switch (type) {
            case PowerUpType::Teleport:  return { 0.6f, 0.2f, 0.9f, alpha * 0.4f };
            case PowerUpType::Bomb:      return { 0.9f, 0.4f, 0.1f, alpha * 0.4f };
            case PowerUpType::Freeze:    return { 0.2f, 0.7f, 0.9f, alpha * 0.4f };
            case PowerUpType::Promotion: return { 0.9f, 0.8f, 0.2f, alpha * 0.4f };
            default: return { 1, 1, 1, 0 };
        }
    }

    Texture& get_texture(PowerUpType type) {
        switch (type) {
            case PowerUpType::Teleport:  return _tex_teleport;
            case PowerUpType::Bomb:      return _tex_bomb;
            case PowerUpType::Freeze:    return _tex_freeze;
            case PowerUpType::Promotion: return _tex_promotion;
            default: return _tex_teleport;
        }
    }

    Texture _tex_teleport, _tex_bomb, _tex_freeze, _tex_promotion;
};
