#pragma once

struct ActionBar {
    float charges = 4.0f;
    float max_charges = 4.0f;
    float recharge_rate = 0.3f;
    float freeze_timer = 0.0f;

    void update(float delta_time) {
        if (freeze_timer > 0.0f) {
            freeze_timer -= delta_time;
            return;
        }
        charges = std::min(max_charges, charges + recharge_rate * delta_time);
    }

    bool can_move() const {
        return charges >= 1.0f && freeze_timer <= 0.0f;
    }

    bool is_frozen() const {
        return freeze_timer > 0.0f;
    }

    void freeze(float duration) {
        freeze_timer = duration;
    }

    void consume() {
        charges -= 1.0f;
    }

    void refund() {
        charges = std::min(max_charges, charges + 1.0f);
    }

    int full_charges() const {
        return static_cast<int>(charges);
    }

    void set_speed(int preset) {
        switch (preset) {
            case 0: recharge_rate = 0.5f;  break;  // fast
            case 1: recharge_rate = 0.2f;  break;  // normal
            case 2: recharge_rate = 0.1f;  break;  // slow
            default: recharge_rate = 0.2f; break;
        }
    }
};
