#pragma once
#include "board.hpp"
#include "action_bar.hpp"
#include "move_validator.hpp"

enum class PowerUpType : uint8_t {
    None = 0,
    Teleport,
    Bomb,
    Freeze,
    Promotion
};

struct PowerUp {
    PowerUpType type = PowerUpType::None;
    int col = -1;
    int row = -1;
    bool active = false;
};

struct PowerUpManager {
    static constexpr int MAX_POWERUPS = 4;
    static constexpr float SPAWN_DELAY_MIN = 5.0f;
    static constexpr float SPAWN_DELAY_MAX = 15.0f;
    static constexpr float FREEZE_DURATION = 3.5f;

    bool enabled = true;

    void init() {
        for (auto& p : _powerups) { p = PowerUp{}; }
        for (auto& s : _type_spawned) { s = false; }
        _spawn_timer = random_delay();
        _total_spawned = 0;
    }

    void update(float delta_time, const Board& board) {
        if (!enabled) return;
        if (_total_spawned >= MAX_POWERUPS) return;

        _spawn_timer -= delta_time;
        if (_spawn_timer <= 0.0f) {
            try_spawn(board);
            _spawn_timer = random_delay();
        }
    }

    PowerUpType collect(int col, int row) {
        for (auto& p : _powerups) {
            if (p.active && p.col == col && p.row == row) {
                p.active = false;
                return p.type;
            }
        }
        return PowerUpType::None;
    }

    void apply_teleport(Board& board, PieceColor opponent_color) {
        auto [kc, kr] = board.find_king(opponent_color);
        if (kc < 0) return;

        PieceColor attacker = (opponent_color == PieceColor::White)
            ? PieceColor::Black : PieceColor::White;

        // find empty squares where king wouldn't be in check
        std::vector<std::pair<int,int>> safe;
        for (int r = 0; r < Board::SIZE; r++) {
            for (int c = 0; c < Board::SIZE; c++) {
                if (!board.at(c, r).is_empty()) continue;
                if (c == kc && r == kr) continue;

                Board copy = board;
                copy.set(kc, kr, Piece{});
                copy.set(c, r, { PieceType::King, opponent_color, true });
                if (!MoveValidator::is_in_check(copy, opponent_color)) {
                    safe.push_back({ c, r });
                }
            }
        }

        if (safe.empty()) return;

        int idx = std::rand() % safe.size();
        auto [nc, nr] = safe[idx];
        Piece king = board.at(kc, kr);
        board.set(kc, kr, Piece{});
        board.set(nc, nr, king);
    }

    void apply_bomb(Board& board, PieceColor opponent_color) {
        std::vector<std::pair<int,int>> targets;
        for (int r = 0; r < Board::SIZE; r++) {
            for (int c = 0; c < Board::SIZE; c++) {
                const Piece& p = board.at(c, r);
                if (p.is_color(opponent_color) && p.type != PieceType::King) {
                    targets.push_back({ c, r });
                }
            }
        }
        if (targets.empty()) return;

        int idx = std::rand() % targets.size();
        auto [tc, tr] = targets[idx];
        board.set(tc, tr, Piece{});
    }

    void apply_freeze(ActionBar& opponent_bar) {
        opponent_bar.freeze(FREEZE_DURATION);
    }

    void apply_promotion(Board& board, int col, int row) {
        Piece& p = board.at(col, row);
        if (p.type != PieceType::Queen && p.type != PieceType::King && !p.is_empty()) {
            p.type = PieceType::Queen;
        }
    }

    bool has_powerup(int col, int row) const {
        for (const auto& p : _powerups) {
            if (p.active && p.col == col && p.row == row) return true;
        }
        return false;
    }

    const PowerUp* get_powerups() const { return _powerups; }

private:
    float random_delay() {
        float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return SPAWN_DELAY_MIN + t * (SPAWN_DELAY_MAX - SPAWN_DELAY_MIN);
    }

    void try_spawn(const Board& board) {
        int slot = -1;
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (!_powerups[i].active) { slot = i; break; }
        }
        if (slot < 0) return;

        // only spawn in the middle of the board
        std::vector<std::pair<int,int>> candidates;
        for (int r = 2; r <= 5; r++) {
            for (int c = 1; c <= 6; c++) {
                if (board.at(c, r).is_empty() && !has_powerup(c, r)) {
                    candidates.push_back({ c, r });
                }
            }
        }
        if (candidates.empty()) return;

        int pos_idx = std::rand() % candidates.size();
        auto [sc, sr] = candidates[pos_idx];

        // each type only once
        std::vector<int> available;
        for (int t = 1; t <= 4; t++) {
            if (!_type_spawned[t]) available.push_back(t);
        }
        if (available.empty()) return;

        PowerUpType type = static_cast<PowerUpType>(available[std::rand() % available.size()]);
        _type_spawned[static_cast<int>(type)] = true;

        _powerups[slot] = { type, sc, sr, true };
        _total_spawned++;
        //std::println("spawned powerup type {} at ({},{})", (int)type, sc, sr);
    }

    PowerUp _powerups[MAX_POWERUPS] = {};
    bool _type_spawned[5] = {}; // index by PowerUpType (1-4)
    float _spawn_timer = 0.0f;
    int _total_spawned = 0;
};
