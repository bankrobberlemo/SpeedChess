#pragma once

enum class PieceType : uint8_t {
    None = 0,
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King
};

enum class PieceColor : uint8_t {
    White = 0,
    Black = 1
};

struct Piece {
    PieceType type = PieceType::None;
    PieceColor color = PieceColor::White;
    bool has_moved = false; // for castling + pawn double step

    bool is_empty() const { return type == PieceType::None; }
    bool is_color(PieceColor c) const { return !is_empty() && color == c; }
};
