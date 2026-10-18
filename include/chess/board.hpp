#pragma once
#include "piece.hpp"

struct Board {
    static constexpr int SIZE = 8;

    void init() {
        for (auto& sq : squares) sq = Piece{};
        ep_col = -1;
        ep_row = -1;
        ep_color = PieceColor::White;

        // pawns
        for (int col = 0; col < SIZE; col++) {
            set(col, 1, { PieceType::Pawn, PieceColor::Black });
            set(col, 6, { PieceType::Pawn, PieceColor::White });
        }

        // black (row 0)
        set(0, 0, { PieceType::Rook,   PieceColor::Black });
        set(1, 0, { PieceType::Knight, PieceColor::Black });
        set(2, 0, { PieceType::Bishop, PieceColor::Black });
        set(3, 0, { PieceType::Queen,  PieceColor::Black });
        set(4, 0, { PieceType::King,   PieceColor::Black });
        set(5, 0, { PieceType::Bishop, PieceColor::Black });
        set(6, 0, { PieceType::Knight, PieceColor::Black });
        set(7, 0, { PieceType::Rook,   PieceColor::Black });

        // white (row 7)
        set(0, 7, { PieceType::Rook,   PieceColor::White });
        set(1, 7, { PieceType::Knight, PieceColor::White });
        set(2, 7, { PieceType::Bishop, PieceColor::White });
        set(3, 7, { PieceType::Queen,  PieceColor::White });
        set(4, 7, { PieceType::King,   PieceColor::White });
        set(5, 7, { PieceType::Bishop, PieceColor::White });
        set(6, 7, { PieceType::Knight, PieceColor::White });
        set(7, 7, { PieceType::Rook,   PieceColor::White });
    }

    auto at(int col, int row) const -> const Piece& {
        return squares[row * SIZE + col];
    }
    auto at(int col, int row) -> Piece& {
        return squares[row * SIZE + col];
    }
    void set(int col, int row, Piece piece) {
        squares[row * SIZE + col] = piece;
    }

    static bool in_bounds(int col, int row) {
        return col >= 0 && col < SIZE && row >= 0 && row < SIZE;
    }

    // en passant
    int ep_col = -1;
    int ep_row = -1;
    PieceColor ep_color = PieceColor::White;

    auto move_piece(int from_col, int from_row, int to_col, int to_row) -> Piece {
        Piece moving = at(from_col, from_row);
        Piece captured = at(to_col, to_row);

        // en passant capture
        if (moving.type == PieceType::Pawn && to_col == ep_col && to_row == ep_row) {
            int captured_row = (moving.color == PieceColor::White) ? to_row + 1 : to_row - 1;
            captured = at(to_col, captured_row);
            set(to_col, captured_row, Piece{});
        }

        // update en passant
        ep_col = -1;
        ep_row = -1;
        if (moving.type == PieceType::Pawn && std::abs(to_row - from_row) == 2) {
            ep_col = from_col;
            ep_row = (from_row + to_row) / 2;
            ep_color = moving.color;
        }

        // castling
        if (moving.type == PieceType::King && std::abs(to_col - from_col) == 2) {
            if (to_col > from_col) {
                // kingside
                Piece rook = at(7, from_row);
                rook.has_moved = true;
                set(5, from_row, rook);
                set(7, from_row, Piece{});
            } else {
                // queenside
                Piece rook = at(0, from_row);
                rook.has_moved = true;
                set(3, from_row, rook);
                set(0, from_row, Piece{});
            }
        }

        moving.has_moved = true;
        set(to_col, to_row, moving);
        set(from_col, from_row, Piece{});

        // auto-promote pawn
        Piece& landed = at(to_col, to_row);
        if (landed.type == PieceType::Pawn) {
            if ((landed.color == PieceColor::White && to_row == 0) ||
                (landed.color == PieceColor::Black && to_row == 7)) {
                landed.type = PieceType::Queen;
            }
        }

        return captured;
    }

    bool king_exists(PieceColor color) const {
        for (const auto& sq : squares) {
            if (sq.type == PieceType::King && sq.color == color) return true;
        }
        return false;
    }

    auto find_king(PieceColor color) const -> std::pair<int, int> {
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                const auto& p = at(col, row);
                if (p.type == PieceType::King && p.color == color) {
                    return { col, row };
                }
            }
        }
        return { -1, -1 };
    }

    std::array<Piece, 64> squares{};
};
