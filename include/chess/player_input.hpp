#pragma once
#include "../input.hpp"
#include "board.hpp"

struct PlayerInput {
    SDL_Keycode key_up, key_down, key_left, key_right;
    SDL_Keycode key_select;
    SDL_Keycode key_cancel;

    int cursor_col = 0;
    int cursor_row = 0;

    bool has_selection = false;
    int selected_col = -1;
    int selected_row = -1;

    PieceColor color = PieceColor::White;

    bool move_requested = false;
    int move_to_col = -1;
    int move_to_row = -1;

    void init_player1() {
        color = PieceColor::White;
        key_up    = SDLK_UP;
        key_down  = SDLK_DOWN;
        key_left  = SDLK_LEFT;
        key_right = SDLK_RIGHT;
        key_select = SDLK_RETURN;
        key_cancel = SDLK_DELETE;
        cursor_col = 4;
        cursor_row = 7;
    }

    void init_player2() {
        color = PieceColor::Black;
        key_up    = SDLK_W;
        key_down  = SDLK_S;
        key_left  = SDLK_A;
        key_right = SDLK_D;
        key_select = SDLK_SPACE;
        key_cancel = SDLK_TAB;
        cursor_col = 4;
        cursor_row = 0;
    }

    void update(const Board& board, const std::vector<std::pair<int,int>>& valid_moves) {
        move_requested = false;

        if (Keys::pressed(key_up))    cursor_row = std::max(0, cursor_row - 1);
        if (Keys::pressed(key_down))  cursor_row = std::min(Board::SIZE - 1, cursor_row + 1);
        if (Keys::pressed(key_left))  cursor_col = std::max(0, cursor_col - 1);
        if (Keys::pressed(key_right)) cursor_col = std::min(Board::SIZE - 1, cursor_col + 1);

        if (Keys::pressed(key_cancel)) {
            has_selection = false;
            selected_col = -1;
            selected_row = -1;
        }

        if (Keys::pressed(key_select)) {
            if (!has_selection) {
                const Piece& p = board.at(cursor_col, cursor_row);
                if (p.is_color(color)) {
                    has_selection = true;
                    selected_col = cursor_col;
                    selected_row = cursor_row;
                    //std::println("selected piece at ({},{})", cursor_col, cursor_row);
                }
            } else {
                const Piece& p = board.at(cursor_col, cursor_row);
                if (p.is_color(color)) {
                    // switch to different own piece
                    selected_col = cursor_col;
                    selected_row = cursor_row;
                } else {
                    for (auto [mc, mr] : valid_moves) {
                        if (mc == cursor_col && mr == cursor_row) {
                            move_requested = true;
                            move_to_col = cursor_col;
                            move_to_row = cursor_row;
                            break;
                        }
                    }
                    if (!move_requested) {
                        has_selection = false;
                        selected_col = -1;
                        selected_row = -1;
                    }
                }
            }
        }
    }

    void clear_selection() {
        has_selection = false;
        selected_col = -1;  // could use 0 here but -1 is safer
        selected_row = -1;
        move_requested = false;
    }
};
