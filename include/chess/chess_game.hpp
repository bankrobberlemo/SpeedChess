#pragma once
#include "../renderer2d.hpp"
#include "board.hpp"
#include "board_renderer.hpp"
#include "piece_renderer.hpp"
#include "move_validator.hpp"
#include "player_input.hpp"
#include "action_bar.hpp"
#include "bitmap_font.hpp"
#include "audio_manager.hpp"
#include "powerup.hpp"
#include "powerup_renderer.hpp"

enum class GameState {
    MainMenu,
    Settings,
    Playing,
    Paused,
    GameOver
};

struct ChessGame {
    void init() {
        _renderer.init();
        _piece_renderer.init();
        _powerup_renderer.init();
        _font.init("font.png");
        _audio.init();

        _players[0].init_player1();
        _players[1].init_player2();

        _state = GameState::MainMenu;
        _audio.play_menu_music();
    }
    void destroy() {
        _audio.destroy();
        _font.destroy();
        _powerup_renderer.destroy();
        _piece_renderer.destroy();
        _renderer.destroy();
    }

    void handle_inputs() {
        switch (_state) {
            case GameState::MainMenu: handle_menu_input(); break;
            case GameState::Settings: handle_settings_input(); break;
            case GameState::Playing:  handle_game_input(); break;
            case GameState::Paused:   handle_pause_input(); break;
            case GameState::GameOver: handle_gameover_input(); break;
        }
    }

    void update(float delta_time) {
        _audio.update();
        _game_time += delta_time;

        if (_state == GameState::Playing) {
            _action_bars[0].update(delta_time);
            _action_bars[1].update(delta_time);
            for (int i = 0; i < 2; i++) {
                if (_flash_timer[i] > 0.0f) _flash_timer[i] -= delta_time;
            }

            // piece slide animations
            for (auto& a : _anims) {
                if (!a.active) continue;
                a.progress += delta_time / 0.15f;
                if (a.progress >= 1.0f) a.active = false;
            }

            int old_count = count_active_powerups();
            _powerups.update(delta_time, _board);
            if (count_active_powerups() > old_count) {
                _audio.play_pu_spawn();
            }

            for (int i = 0; i < 2; i++) {
                process_player_move(i);
            }

            // check if a king got captured
            if (!_board.king_exists(PieceColor::White)) {
                _state = GameState::GameOver;
                _winner = PieceColor::Black;
                _audio.play_victory();
            } else if (!_board.king_exists(PieceColor::Black)) {
                _state = GameState::GameOver;
                _winner = PieceColor::White;
                _audio.play_victory();
            }
        }
    }

    void draw(float screen_w = 1280.0f, float screen_h = 720.0f) {
        _screen_w = screen_w;
        _screen_h = screen_h;
        _board_renderer.update_origin(screen_w, screen_h);
        _renderer.begin(screen_w, screen_h);

        switch (_state) {
            case GameState::MainMenu: draw_menu(); break;
            case GameState::Settings: draw_settings(); break;
            case GameState::Playing:  draw_game(); break;
            case GameState::Paused:   draw_game(); draw_pause(); break;
            case GameState::GameOver: draw_gameover(); break;
        }

        _renderer.end();
    }

private:
    void handle_menu_input() {
        if (Keys::pressed(SDLK_UP) || Keys::pressed(SDLK_W)) {
            _menu_selection = (_menu_selection + 3) % 4;
            _audio.play_select();
        }
        if (Keys::pressed(SDLK_DOWN) || Keys::pressed(SDLK_S)) {
            _menu_selection = (_menu_selection + 1) % 4;
            _audio.play_select();
        }

        if (_menu_selection == 0) {
            if (Keys::pressed(SDLK_LEFT) || Keys::pressed(SDLK_A)) {
                _speed_preset = std::max(0, _speed_preset - 1);
                _audio.play_speed();
            }
            if (Keys::pressed(SDLK_RIGHT) || Keys::pressed(SDLK_D)) {
                _speed_preset = std::min(2, _speed_preset + 1);
                _audio.play_speed();
            }
        }

        if (_menu_selection == 1) {
            if (Keys::pressed(SDLK_LEFT) || Keys::pressed(SDLK_RIGHT) ||
                Keys::pressed(SDLK_A) || Keys::pressed(SDLK_D) ||
                Keys::pressed(SDLK_RETURN) || Keys::pressed(SDLK_SPACE)) {
                _powerups_enabled = !_powerups_enabled;
                _audio.play_select();
            }
        }

        if (Keys::pressed(SDLK_RETURN) || Keys::pressed(SDLK_SPACE)) {
            if (_menu_selection == 2) {
                start_game();
            } else if (_menu_selection == 3) {
                _state = GameState::Settings;
                _settings_selection = 0;
                _audio.play_select();
            }
        }
    }

    void handle_settings_input() {
        if (Keys::pressed(SDLK_UP) || Keys::pressed(SDLK_W)) {
            _settings_selection = std::max(0, _settings_selection - 1);
            _audio.play_select();
        }
        if (Keys::pressed(SDLK_DOWN) || Keys::pressed(SDLK_S)) {
            _settings_selection = std::min(2, _settings_selection + 1);
            _audio.play_select();
        }

        float step = 0.1f;
        if (Keys::pressed(SDLK_LEFT) || Keys::pressed(SDLK_A)) {
            if (_settings_selection == 0) _audio.adjust_music_volume(-step);
            if (_settings_selection == 1) _audio.adjust_sound_volume(-step);
        }
        if (Keys::pressed(SDLK_RIGHT) || Keys::pressed(SDLK_D)) {
            if (_settings_selection == 0) _audio.adjust_music_volume(step);
            if (_settings_selection == 1) _audio.adjust_sound_volume(step);
        }

        if (_settings_selection == 2 && (Keys::pressed(SDLK_RETURN) || Keys::pressed(SDLK_SPACE))) {
            _state = GameState::MainMenu;
            _audio.play_select();
        }
        if (Keys::pressed(SDLK_ESCAPE)) {
            _state = GameState::MainMenu;
            _audio.play_select();
        }
    }

    void handle_game_input() {
        if (Keys::pressed(SDLK_ESCAPE)) {
            _state = GameState::Paused;
            _pause_selection = 0;
            _audio.play_select();
            return;
        }

        for (int i = 0; i < 2; i++) {
            // if selected piece was captured by opponent, clear selection
            if (_players[i].has_selection) {
                const Piece& sel = _board.at(_players[i].selected_col, _players[i].selected_row);
                if (!sel.is_color(_players[i].color)) {
                    _players[i].clear_selection();
                }
            }

            std::vector<std::pair<int,int>> valid;
            if (_players[i].has_selection) {
                valid = MoveValidator::get_valid_moves(_board,
                    _players[i].selected_col, _players[i].selected_row);
            }

            bool had_selection = _players[i].has_selection;
            _valid_moves[i] = valid;
            _players[i].update(_board, _valid_moves[i]);

            if (!had_selection && _players[i].has_selection) {
                _audio.play_select();
            }
        }
    }

    void handle_pause_input() {
        if (Keys::pressed(SDLK_ESCAPE)) {
            _state = GameState::Playing;
            _audio.play_select();
            return;
        }

        if (Keys::pressed(SDLK_UP) || Keys::pressed(SDLK_W)) {
            _pause_selection = std::max(0, _pause_selection - 1);
            _audio.play_select();
        }
        if (Keys::pressed(SDLK_DOWN) || Keys::pressed(SDLK_S)) {
            _pause_selection = std::min(1, _pause_selection + 1);
            _audio.play_select();
        }

        if (Keys::pressed(SDLK_RETURN) || Keys::pressed(SDLK_SPACE)) {
            if (_pause_selection == 0) {
                _state = GameState::Playing;
                _audio.play_select();
            } else {
                _state = GameState::MainMenu;
                _audio.play_select();
                _audio.play_menu_music();
            }
        }
    }

    void handle_gameover_input() {
        if (Keys::pressed(SDLK_RETURN) || Keys::pressed(SDLK_SPACE)) {
            _state = GameState::MainMenu;
            _audio.play_menu_music();
        }
    }

    void start_game() {
        _board.init();
        _players[0].init_player1();
        _players[1].init_player2();

        for (int i = 0; i < 2; i++) {
            _action_bars[i] = ActionBar{};
            _action_bars[i].set_speed(_speed_preset);
            _valid_moves[i].clear();
        }

        _powerups = PowerUpManager{};
        _powerups.enabled = _powerups_enabled;
        _powerups.init();
        _game_time = 0.0f;
        _flash_timer[0] = 0.0f;
        _flash_timer[1] = 0.0f;
        for (auto& a : _anims) a.active = false;

        _state = GameState::Playing;
        _audio.play_start();
        _audio.play_gameplay_music();
    }

    void process_player_move(int player_index) {
        auto& player = _players[player_index];
        auto& bar = _action_bars[player_index];

        if (!player.move_requested) return;

        if (!bar.can_move()) {
            player.move_requested = false;
            _flash_timer[player_index] = 0.3f;
            return;
        }

        auto current_valid = MoveValidator::get_valid_moves(_board,
            player.selected_col, player.selected_row);

        bool still_valid = false;
        for (auto [mc, mr] : current_valid) {
            if (mc == player.move_to_col && mr == player.move_to_row) {
                still_valid = true;
                break;
            }
        }

        if (still_valid) {
            Piece moving = _board.at(player.selected_col, player.selected_row);
            Piece target = _board.at(player.move_to_col, player.move_to_row);
            bool is_capture = !target.is_empty();
            bool is_en_passant = (moving.type == PieceType::Pawn &&
                player.move_to_col == _board.ep_col && player.move_to_row == _board.ep_row);
            bool is_castle = (moving.type == PieceType::King &&
                std::abs(player.move_to_col - player.selected_col) == 2);
            bool is_promotion = (moving.type == PieceType::Pawn &&
                ((moving.color == PieceColor::White && player.move_to_row == 0) ||
                 (moving.color == PieceColor::Black && player.move_to_row == 7)));

            auto [fx, fy] = _board_renderer.board_to_screen(player.selected_col, player.selected_row);
            auto [tx, ty] = _board_renderer.board_to_screen(player.move_to_col, player.move_to_row);

            _board.move_piece(player.selected_col, player.selected_row,
                              player.move_to_col, player.move_to_row);
            bar.consume();
            //std::println("p{} moved {},{} -> {},{}", player_index, player.selected_col, player.selected_row, player.move_to_col, player.move_to_row);

            for (auto& a : _anims) {
                if (!a.active) {
                    a = { true, moving.type, moving.color,
                          player.move_to_col, player.move_to_row,
                          fx, fy, tx, ty, 0.0f };
                    break;
                }
            }

            PowerUpType pu = _powerups.collect(player.move_to_col, player.move_to_row);
            if (pu != PowerUpType::None) {
                apply_powerup(pu, player_index, player.move_to_col, player.move_to_row);
            }

            if (is_castle) {
                _audio.play_castle();
            } else if (is_promotion) {
                _audio.play_promote();
            } else if (is_capture || is_en_passant) {
                _audio.play_capture();
            } else {
                _audio.play_move();
            }

            PieceColor opponent = (player.color == PieceColor::White)
                ? PieceColor::Black : PieceColor::White;
            if (MoveValidator::is_in_check(_board, opponent)) {
                _audio.play_check();
            }
        }

        player.clear_selection();
    }

    void apply_powerup(PowerUpType type, int player_index, int col, int row) {
        PieceColor player_color = _players[player_index].color;
        PieceColor opponent_color = (player_color == PieceColor::White)
            ? PieceColor::Black : PieceColor::White;
        int opponent_index = 1 - player_index;

        switch (type) {
            case PowerUpType::Teleport:
                _powerups.apply_teleport(_board, opponent_color);
                _audio.play_pu_teleport();
                break;
            case PowerUpType::Bomb:
                _powerups.apply_bomb(_board, opponent_color);
                _audio.play_pu_bomb();
                break;
            case PowerUpType::Freeze:
                _powerups.apply_freeze(_action_bars[opponent_index]);
                _audio.play_pu_freeze();
                break;
            case PowerUpType::Promotion:
                _powerups.apply_promotion(_board, col, row);
                _audio.play_pu_promotion();
                break;
            default: break;
        }
    }

    int count_active_powerups() {
        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (_powerups.get_powerups()[i].active) count++;
        }
        return count;
    }

    // draw functions

    void draw_menu() {
        float title_w = _font.measure_width("SPEED CHESS", 4.0f);
        _font.draw_text(_renderer, "SPEED CHESS", (_screen_w - title_w) / 2.0f, 120.0f, 4.0f);

        glm::vec4 sel(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 dim(0.5f, 0.5f, 0.5f, 1.0f);

        const char* speeds[] = { "< FAST >", "< NORMAL >", "< SLOW >" };
        std::string speed_label = "SPEED: " + std::string(speeds[_speed_preset]);
        float sw = _font.measure_width(speed_label, 2.0f);
        _font.draw_text(_renderer, speed_label, (_screen_w - sw) / 2.0f, 260.0f, 2.0f,
            _menu_selection == 0 ? glm::vec4(1.0f, 0.8f, 0.2f, 1.0f) : dim);

        std::string pu_label = std::string("POWER-UPS: ") + (_powerups_enabled ? "ON" : "OFF");
        float pw = _font.measure_width(pu_label, 2.0f);
        glm::vec4 pu_color = _menu_selection == 1
            ? (_powerups_enabled ? glm::vec4(0.3f, 1.0f, 0.3f, 1.0f) : glm::vec4(1.0f, 0.3f, 0.3f, 1.0f))
            : dim;
        _font.draw_text(_renderer, pu_label, (_screen_w - pw) / 2.0f, 310.0f, 2.0f, pu_color);

        std::string start_d = (_menu_selection == 2) ? "> START GAME <" : "  START GAME  ";
        float sd_w = _font.measure_width(start_d, 2.5f);
        _font.draw_text(_renderer, start_d, (_screen_w - sd_w) / 2.0f, 400.0f, 2.5f,
            _menu_selection == 2 ? sel : dim);

        std::string set_d = (_menu_selection == 3) ? "> SETTINGS <" : "  SETTINGS  ";
        float se_w = _font.measure_width(set_d, 2.0f);
        _font.draw_text(_renderer, set_d, (_screen_w - se_w) / 2.0f, 460.0f, 2.0f,
            _menu_selection == 3 ? sel : dim);

        float p1_hw = _font.measure_width("P1: ARROWS + ENTER", 1.5f);
        _font.draw_text(_renderer, "P1: ARROWS + ENTER", _screen_w * 0.25f - p1_hw / 2.0f, 580.0f, 1.5f,
            glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
        float p2_hw = _font.measure_width("P2: WASD + SPACE", 1.5f);
        _font.draw_text(_renderer, "P2: WASD + SPACE", _screen_w * 0.75f - p2_hw / 2.0f, 580.0f, 1.5f,
            glm::vec4(1.0f, 0.4f, 0.4f, 1.0f));
    }

    void draw_settings() {
        float title_w = _font.measure_width("SETTINGS", 3.0f);
        _font.draw_text(_renderer, "SETTINGS", (_screen_w - title_w) / 2.0f, 150.0f, 3.0f);

        float cx = _screen_w / 2.0f;
        float y_start = 280.0f;
        float y_step = 80.0f;

        glm::vec4 sel(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 dim(0.5f, 0.5f, 0.5f, 1.0f);

        draw_volume_option("MUSIC", _audio.music_volume, cx, y_start,
            _settings_selection == 0 ? sel : dim);
        draw_volume_option("SOUND", _audio.sound_volume, cx, y_start + y_step,
            _settings_selection == 1 ? sel : dim);

        std::string back_text = (_settings_selection == 2) ? "> BACK <" : "  BACK  ";
        float back_w = _font.measure_width(back_text, 2.0f);
        _font.draw_text(_renderer, back_text, (_screen_w - back_w) / 2.0f, y_start + y_step * 2 + 20.0f, 2.0f,
            _settings_selection == 2 ? sel : dim);

        float hint_w = _font.measure_width("LEFT/RIGHT TO ADJUST  -  ESC TO GO BACK", 1.2f);
        _font.draw_text(_renderer, "LEFT/RIGHT TO ADJUST  -  ESC TO GO BACK",
            (_screen_w - hint_w) / 2.0f, 580.0f, 1.2f, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
    }

    void draw_volume_option(const std::string& label, float volume, float cx, float y, glm::vec4 color) {
        float label_w = _font.measure_width(label, 2.0f);
        _font.draw_text(_renderer, label, cx - label_w - 20.0f, y, 2.0f, color);

        float bar_x = cx + 20.0f;
        float bar_w = 200.0f;
        float bar_h = 24.0f;
        _renderer.draw_solid(bar_x, y + 4.0f, bar_w, bar_h, glm::vec4(0.2f, 0.2f, 0.2f, 0.8f));

        float fill_w = bar_w * volume;
        if (fill_w > 0.0f) {
            _renderer.draw_solid(bar_x, y + 4.0f, fill_w, bar_h, glm::vec4(color.r, color.g, color.b, 0.7f));
        }

        int pct = static_cast<int>(volume * 100.0f + 0.5f);
        std::string pct_text = std::to_string(pct) + "%";
        _font.draw_text(_renderer, pct_text, bar_x + bar_w + 15.0f, y + 4.0f, 1.5f, color);
    }

    void draw_game() {
        _board_renderer.draw(_renderer);
        _powerup_renderer.draw(_renderer, _board_renderer, _powerups, _game_time);

        for (int i = 0; i < 2; i++) {
            if (_players[i].has_selection) {
                glm::vec4 select_color = (i == 0)
                    ? glm::vec4(0.2f, 0.4f, 1.0f, 0.6f)
                    : glm::vec4(1.0f, 0.2f, 0.2f, 0.6f);
                _board_renderer.draw_highlight(_renderer,
                    _players[i].selected_col, _players[i].selected_row, select_color);

                glm::vec4 move_dot = (i == 0)
                    ? glm::vec4(0.2f, 0.4f, 1.0f, 0.5f)
                    : glm::vec4(1.0f, 0.25f, 0.25f, 0.5f);
                glm::vec4 capture_hl = (i == 0)
                    ? glm::vec4(0.2f, 0.4f, 1.0f, 0.45f)
                    : glm::vec4(1.0f, 0.25f, 0.25f, 0.45f);

                for (auto [mc, mr] : _valid_moves[i]) {
                    if (_board.at(mc, mr).is_empty()) {
                        _board_renderer.draw_dot(_renderer, mc, mr, move_dot);
                    } else {
                        _board_renderer.draw_highlight(_renderer, mc, mr, capture_hl);
                        _board_renderer.draw_border(_renderer, mc, mr, capture_hl * 1.5f, 3.0f);
                    }
                }
            }
        }

        glm::vec4 cursor_colors[2] = {
            glm::vec4(0.2f, 0.5f, 1.0f, 0.9f),
            glm::vec4(1.0f, 0.3f, 0.3f, 0.9f),
        };
        for (int i = 0; i < 2; i++) {
            _board_renderer.draw_border(_renderer,
                _players[i].cursor_col, _players[i].cursor_row, cursor_colors[i], 4.0f);
        }

        _piece_renderer.draw_board_skip(_renderer, _board_renderer, _board, _anims, MAX_ANIMS);

        // lerp animated pieces
        for (const auto& a : _anims) {
            if (!a.active) continue;
            float t = a.progress;
            t = t * t * (3.0f - 2.0f * t);
            float x = a.from_x + (a.to_x - a.from_x) * t;
            float y = a.from_y + (a.to_y - a.from_y) * t;
            _piece_renderer.draw_piece_at(_renderer, a.type, a.color, x, y);
        }

        draw_action_bars();

        // red highlight if king is in check
        for (int i = 0; i < 2; i++) {
            if (MoveValidator::is_in_check(_board, _players[i].color)) {
                auto [kc, kr] = _board.find_king(_players[i].color);
                _board_renderer.draw_highlight(_renderer, kc, kr,
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.4f));
            }
        }

        float board_left = _board_renderer.origin_x;
        float board_right = board_left + BoardRenderer::BOARD_SIZE;

        float p1w = _font.measure_width("WHITE (P1)", 1.5f);
        _font.draw_text(_renderer, "WHITE (P1)",
            (board_left - p1w) / 2.0f, 10.0f, 1.5f,
            glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
        _font.draw_text(_renderer, "BLACK (P2)",
            board_right + (_screen_w - board_right - _font.measure_width("BLACK (P2)", 1.5f)) / 2.0f,
            10.0f, 1.5f, glm::vec4(1.0f, 0.4f, 0.4f, 1.0f));

        int minutes = (int)_game_time / 60;
        int seconds = (int)_game_time % 60;
        char timer_buf[8];
        std::snprintf(timer_buf, sizeof(timer_buf), "%02d:%02d", minutes, seconds);
        float timer_w = _font.measure_width(timer_buf, 2.0f);
        _font.draw_text(_renderer, timer_buf, (_screen_w - timer_w) / 2.0f, 10.0f, 2.0f,
            glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    void draw_action_bars() {
        float bar_width = 200.0f;
        float bar_height = 30.0f;
        float segment_gap = 4.0f;
        float segment_width = (bar_width - segment_gap * 3) / 4.0f;

        float board_left = _board_renderer.origin_x;
        float board_right = board_left + BoardRenderer::BOARD_SIZE;
        float bar_x[2] = {
            (board_left - bar_width) / 2.0f,
            board_right + (_screen_w - board_right - bar_width) / 2.0f
        };
        float bar_y = _screen_h - 30.0f;

        for (int i = 0; i < 2; i++) {
            glm::vec4 bar_bg(0.2f, 0.2f, 0.2f, 0.8f);
            if (_flash_timer[i] > 0.0f) {
                float t = _flash_timer[i] / 0.3f;
                bar_bg = glm::mix(bar_bg, glm::vec4(0.8f, 0.1f, 0.1f, 0.9f), t);
            }
            _renderer.draw_solid(bar_x[i], bar_y, bar_width, bar_height, bar_bg);

            for (int seg = 0; seg < 4; seg++) {
                float sx = bar_x[i] + seg * (segment_width + segment_gap);
                float fill = std::min(1.0f, std::max(0.0f, _action_bars[i].charges - seg));

                if (fill > 0.0f) {
                    glm::vec4 seg_color;
                    if (_action_bars[i].is_frozen()) {
                        seg_color = glm::vec4(0.3f, 0.7f, 0.9f, 0.9f); // ice
                    } else {
                        seg_color = (i == 0)
                            ? glm::vec4(0.3f, 0.5f, 1.0f, 0.9f)
                            : glm::vec4(1.0f, 0.3f, 0.3f, 0.9f);
                    }
                    _renderer.draw_solid(sx, bar_y, segment_width * fill, bar_height, seg_color);
                }

                // segment borders
                _renderer.draw_solid(sx, bar_y, segment_width, 2.0f,
                    glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
                _renderer.draw_solid(sx, bar_y + bar_height - 2.0f, segment_width, 2.0f,
                    glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
            }

            if (_action_bars[i].is_frozen()) {
                float freeze_w = _font.measure_width("FROZEN", 1.5f);
                _font.draw_text(_renderer, "FROZEN",
                    bar_x[i] + (bar_width - freeze_w) / 2.0f, bar_y - 22.0f, 1.5f,
                    glm::vec4(0.3f, 0.8f, 1.0f, 1.0f));
            }
        }
    }

    void draw_pause() {
        _renderer.draw_solid(0, 0, _screen_w, _screen_h, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));

        // panel
        float panel_w = 500.0f;
        float panel_h = 300.0f;
        float panel_x = (_screen_w - panel_w) / 2.0f;
        float panel_y = 190.0f;
        _renderer.draw_solid(panel_x, panel_y, panel_w, panel_h,
            glm::vec4(0.08f, 0.08f, 0.12f, 0.95f));
        float b = 2.0f;
        _renderer.draw_solid(panel_x, panel_y, panel_w, b, glm::vec4(0.4f, 0.4f, 0.5f, 0.8f));
        _renderer.draw_solid(panel_x, panel_y + panel_h - b, panel_w, b, glm::vec4(0.4f, 0.4f, 0.5f, 0.8f));
        _renderer.draw_solid(panel_x, panel_y, b, panel_h, glm::vec4(0.4f, 0.4f, 0.5f, 0.8f));
        _renderer.draw_solid(panel_x + panel_w - b, panel_y, b, panel_h, glm::vec4(0.4f, 0.4f, 0.5f, 0.8f));

        float title_w = _font.measure_width("PAUSED", 4.0f);
        _font.draw_text(_renderer, "PAUSED", (_screen_w - title_w) / 2.0f, 220.0f, 4.0f);

        glm::vec4 sel(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 dim(0.5f, 0.5f, 0.5f, 1.0f);

        std::string resume_text = (_pause_selection == 0) ? "> RESUME <" : "  RESUME  ";
        float rw = _font.measure_width(resume_text, 2.5f);
        _font.draw_text(_renderer, resume_text, (_screen_w - rw) / 2.0f, 340.0f, 2.5f,
            _pause_selection == 0 ? sel : dim);

        std::string quit_text = (_pause_selection == 1) ? "> QUIT TO MENU <" : "  QUIT TO MENU  ";
        float qw = _font.measure_width(quit_text, 2.5f);
        _font.draw_text(_renderer, quit_text, (_screen_w - qw) / 2.0f, 410.0f, 2.5f,
            _pause_selection == 1 ? glm::vec4(1.0f, 0.4f, 0.4f, 1.0f) : dim);
    }

    void draw_gameover() {
        _renderer.draw_solid(0, 0, _screen_w, _screen_h, glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));

        std::string winner_text = (_winner == PieceColor::White) ? "WHITE WINS!" : "BLACK WINS!";
        glm::vec4 winner_color = (_winner == PieceColor::White)
            ? glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)
            : glm::vec4(1.0f, 0.4f, 0.4f, 1.0f);

        float title_w = _font.measure_width(winner_text, 4.0f);
        _font.draw_text(_renderer, winner_text, (_screen_w - title_w) / 2.0f, 250.0f, 4.0f, winner_color);

        float restart_w = _font.measure_width("PRESS ENTER TO CONTINUE", 2.0f);
        _font.draw_text(_renderer, "PRESS ENTER TO CONTINUE",
            (_screen_w - restart_w) / 2.0f, 400.0f, 2.0f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
    }

    GameState _state = GameState::MainMenu;
    Renderer2D _renderer;
    BoardRenderer _board_renderer;
    PieceRenderer _piece_renderer;
    PowerUpRenderer _powerup_renderer;
    BitmapFont _font;
    AudioManager _audio;
    Board _board;
    PlayerInput _players[2];
    ActionBar _action_bars[2];
    PowerUpManager _powerups;
    std::vector<std::pair<int,int>> _valid_moves[2];
    PieceColor _winner = PieceColor::White;
    float _game_time = 0.0f;
    float _flash_timer[2] = { 0.0f, 0.0f };
    static constexpr int MAX_ANIMS = 4;
    PieceAnim _anims[MAX_ANIMS];
    float _screen_w = 1280.0f;
    float _screen_h = 720.0f;
    int _speed_preset = 1; // 0=fast, 1=normal, 2=slow
    int _menu_selection = 0;
    int _settings_selection = 0;
    int _pause_selection = 0;
    bool _powerups_enabled = true;
};
