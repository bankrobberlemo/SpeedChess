#pragma once
#include "../renderer2d.hpp"
#include "board.hpp"
#include "board_renderer.hpp"

struct PieceAnim {
    bool active = false;
    PieceType type = PieceType::None;
    PieceColor color = PieceColor::White;
    int dest_col = 0, dest_row = 0;
    float from_x = 0, from_y = 0;
    float to_x = 0, to_y = 0;
    float progress = 0.0f;
};

struct PieceRenderer {
    void init() {
        std::string names[] = { "", "pawn", "rook", "knight", "bishop", "queen", "king" };
        std::string colors[] = { "w", "b" };

        for (int type = 1; type <= 6; type++) {
            for (int color = 0; color < 2; color++) {
                std::string path = "chess/" + names[type] + "-" + colors[color] + ".png";
                _textures[type][color].init(path);
                _textures[type][color].set_nearest_filter();
            }
        }
        _loaded = true;
    }
    void destroy() {
        if (!_loaded) return;
        for (int type = 1; type <= 6; type++) {
            for (int color = 0; color < 2; color++) {
                _textures[type][color].destroy();
            }
        }
    }

    void draw_piece(Renderer2D& renderer, const BoardRenderer& board_renderer,
                    PieceType type, PieceColor color, int col, int row) {
        if (type == PieceType::None) return;

        auto [sx, sy] = board_renderer.board_to_screen(col, row);
        float padding = 8.0f;
        float piece_size = BoardRenderer::SQUARE_SIZE - padding * 2.0f;

        Texture& tex = _textures[static_cast<int>(type)][static_cast<int>(color)];
        renderer.draw_textured(tex, sx + padding, sy + padding, piece_size, piece_size);
    }

    void draw_piece_at(Renderer2D& renderer, PieceType type, PieceColor color, float sx, float sy) {
        if (type == PieceType::None) return;
        float padding = 8.0f;
        float piece_size = BoardRenderer::SQUARE_SIZE - padding * 2.0f;
        Texture& tex = _textures[static_cast<int>(type)][static_cast<int>(color)];
        renderer.draw_textured(tex, sx + padding, sy + padding, piece_size, piece_size);
    }

    void draw_board(Renderer2D& renderer, const BoardRenderer& board_renderer, const Board& board) {
        for (int row = 0; row < Board::SIZE; row++) {
            for (int col = 0; col < Board::SIZE; col++) {
                const Piece& p = board.at(col, row);
                if (!p.is_empty()) {
                    draw_piece(renderer, board_renderer, p.type, p.color, col, row);
                }
            }
        }
    }

    // same as draw_board but skips animated pieces
    void draw_board_skip(Renderer2D& renderer, const BoardRenderer& board_renderer,
                         const Board& board, const PieceAnim* anims, int max_anims) {
        for (int row = 0; row < Board::SIZE; row++) {
            for (int col = 0; col < Board::SIZE; col++) {
                const Piece& p = board.at(col, row);
                if (p.is_empty()) continue;
                bool skip = false;
                for (int i = 0; i < max_anims; i++) {
                    if (anims[i].active && anims[i].dest_col == col && anims[i].dest_row == row) {
                        skip = true; break;
                    }
                }
                if (!skip) draw_piece(renderer, board_renderer, p.type, p.color, col, row);
            }
        }
    }

    Texture _textures[7][2];
    bool _loaded = false;
};
