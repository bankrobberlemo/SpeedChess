#pragma once
#include "board.hpp"

namespace MoveValidator {

    using MoveList = std::vector<std::pair<int, int>>;

    // forward decl for castling checks
    inline bool is_square_attacked(const Board& board, int col, int row, PieceColor by_color);

    inline auto get_pawn_moves(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        int dir = (color == PieceColor::White) ? -1 : 1;
        int start_row = (color == PieceColor::White) ? 6 : 1;

        // one step forward
        if (Board::in_bounds(col, row + dir) && board.at(col, row + dir).is_empty()) {
            moves.push_back({ col, row + dir });
            // two steps from start
            if (row == start_row && board.at(col, row + 2 * dir).is_empty()) {
                moves.push_back({ col, row + 2 * dir });
            }
        }

        // diagonal captures + en passant
        PieceColor opponent = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        for (int dc : { -1, 1 }) {
            int nc = col + dc;
            int nr = row + dir;
            if (Board::in_bounds(nc, nr)) {
                if (board.at(nc, nr).is_color(opponent)) {
                    moves.push_back({ nc, nr });
                }
                if (nc == board.ep_col && nr == board.ep_row && board.ep_color != color) {
                    moves.push_back({ nc, nr });
                }
            }
        }

        return moves;
    }

    inline auto get_sliding_moves(const Board& board, int col, int row, PieceColor color,
                                   int dc, int dr) -> MoveList {
        MoveList moves;
        int c = col + dc, r = row + dr;
        while (Board::in_bounds(c, r)) {
            if (board.at(c, r).is_empty()) {
                moves.push_back({ c, r });
            } else {
                if (!board.at(c, r).is_color(color)) {
                    moves.push_back({ c, r });
                }
                break;
            }
            c += dc;
            r += dr;
        }
        return moves;
    }

    inline auto get_rook_moves(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        for (auto [dc, dr] : std::vector<std::pair<int,int>>{{ 0,1 }, { 0,-1 }, { 1,0 }, { -1,0 }}) {
            auto m = get_sliding_moves(board, col, row, color, dc, dr);
            moves.insert(moves.end(), m.begin(), m.end());
        }
        return moves;
    }

    inline auto get_bishop_moves(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        for (auto [dc, dr] : std::vector<std::pair<int,int>>{{ 1,1 }, { 1,-1 }, { -1,1 }, { -1,-1 }}) {
            auto m = get_sliding_moves(board, col, row, color, dc, dr);
            moves.insert(moves.end(), m.begin(), m.end());
        }
        return moves;
    }

    inline auto get_queen_moves(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        // straight + diagonal (queen = rook + bishop basically)
        for (auto [dc, dr] : std::vector<std::pair<int,int>>{{ 0,1 }, { 0,-1 }, { 1,0 }, { -1,0 },
                                                              { 1,1 }, { 1,-1 }, { -1,1 }, { -1,-1 }}) {
            auto m = get_sliding_moves(board, col, row, color, dc, dr);
            moves.insert(moves.end(), m.begin(), m.end());
        }
        return moves;
    }

    inline auto get_knight_moves(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        const int offsets[8][2] = {
            {-2,-1}, {-2,1}, {-1,-2}, {-1,2},
            {1,-2}, {1,2}, {2,-1}, {2,1}
        };
        for (auto [dc, dr] : offsets) {
            int nc = col + dc, nr = row + dr;
            if (Board::in_bounds(nc, nr) && !board.at(nc, nr).is_color(color)) {
                moves.push_back({ nc, nr });
            }
        }
        return moves;
    }

    inline auto get_king_moves(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        for (int dc = -1; dc <= 1; dc++) {
            for (int dr = -1; dr <= 1; dr++) {
                if (dc == 0 && dr == 0) continue;
                int nc = col + dc, nr = row + dr;
                if (Board::in_bounds(nc, nr) && !board.at(nc, nr).is_color(color)) {
                    moves.push_back({ nc, nr });
                }
            }
        }

        // castling
        PieceColor opponent = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        const Piece& king = board.at(col, row);
        if (!king.has_moved && !is_square_attacked(board, col, row, opponent)) {
            // kingside
            const Piece& rook_ks = board.at(7, row);
            if (rook_ks.type == PieceType::Rook && rook_ks.is_color(color) && !rook_ks.has_moved) {
                if (board.at(5, row).is_empty() && board.at(6, row).is_empty()) {
                    if (!is_square_attacked(board, 5, row, opponent) &&
                        !is_square_attacked(board, 6, row, opponent)) {
                        moves.push_back({ 6, row });
                    }
                }
            }
            // queenside
            const Piece& rook_qs = board.at(0, row);
            if (rook_qs.type == PieceType::Rook && rook_qs.is_color(color) && !rook_qs.has_moved) {
                if (board.at(1, row).is_empty() && board.at(2, row).is_empty() && board.at(3, row).is_empty()) {
                    if (!is_square_attacked(board, 2, row, opponent) &&
                        !is_square_attacked(board, 3, row, opponent)) {
                        moves.push_back({ 2, row });
                    }
                }
            }
        }

        return moves;
    }

    inline auto get_raw_moves(const Board& board, int col, int row) -> MoveList {
        const Piece& piece = board.at(col, row);
        if (piece.is_empty()) return {};

        switch (piece.type) {
            case PieceType::Pawn:   return get_pawn_moves(board, col, row, piece.color);
            case PieceType::Rook:   return get_rook_moves(board, col, row, piece.color);
            case PieceType::Knight: return get_knight_moves(board, col, row, piece.color);
            case PieceType::Bishop: return get_bishop_moves(board, col, row, piece.color);
            case PieceType::Queen:  return get_queen_moves(board, col, row, piece.color);
            case PieceType::King:   return get_king_moves(board, col, row, piece.color);
            default: return {};
        }
    }

    // king attacks without castling (otherwise is_square_attacked recurses)
    inline auto get_king_attacks(const Board& board, int col, int row, PieceColor color) -> MoveList {
        MoveList moves;
        for (int dc = -1; dc <= 1; dc++) {
            for (int dr = -1; dr <= 1; dr++) {
                if (dc == 0 && dr == 0) continue;
                int nc = col + dc, nr = row + dr;
                if (Board::in_bounds(nc, nr) && !board.at(nc, nr).is_color(color)) {
                    moves.push_back({ nc, nr });
                }
            }
        }
        return moves;
    }

    inline bool is_square_attacked(const Board& board, int col, int row, PieceColor by_color) {
        for (int r = 0; r < Board::SIZE; r++) {
            for (int c = 0; c < Board::SIZE; c++) {
                const Piece& p = board.at(c, r);
                if (!p.is_color(by_color)) continue;

                MoveList moves;
                if (p.type == PieceType::King) {
                    moves = get_king_attacks(board, c, r, by_color);
                } else {
                    moves = get_raw_moves(board, c, r);
                }

                for (auto [mc, mr] : moves) {
                    if (mc == col && mr == row) return true;
                }
            }
        }
        return false;
    }

    inline bool is_in_check(const Board& board, PieceColor color) {
        auto [kc, kr] = board.find_king(color);
        if (kc < 0) return false;
        PieceColor opponent = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        return is_square_attacked(board, kc, kr, opponent);
    }

    inline auto get_valid_moves(const Board& board, int col, int row) -> MoveList {
        const Piece& piece = board.at(col, row);
        if (piece.is_empty()) return {};

        // speed chess: no check restriction, win by capturing the king
        return get_raw_moves(board, col, row);
    }

    inline bool has_valid_moves(const Board& board, PieceColor color) {
        for (int r = 0; r < Board::SIZE; r++) {
            for (int c = 0; c < Board::SIZE; c++) {
                if (board.at(c, r).is_color(color)) {
                    if (!get_valid_moves(board, c, r).empty()) return true;
                }
            }
        }
        return false;
    }
}
