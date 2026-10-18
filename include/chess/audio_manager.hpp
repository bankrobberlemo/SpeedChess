#pragma once

struct AudioClip {
    Uint8* buffer = nullptr;
    Uint32 buffer_size = 0;
    SDL_AudioSpec spec{};

    bool load(const std::string& relative_path) {
        std::string full = std::string(SDL_GetBasePath()) + "assets/audio/" + relative_path;
        if (!SDL_LoadWAV(full.c_str(), &spec, &buffer, &buffer_size)) {
            std::println("WAV load failed: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    void destroy() {
        if (buffer) {
            SDL_free(buffer);
            buffer = nullptr;
        }
    }
};

struct AudioManager {
    void init() {
        // open audio device
        _device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!_device) {
            std::println("Audio device error: {}", SDL_GetError());
            return;
        }
        SDL_GetAudioDeviceFormat(_device, &_device_spec, nullptr);
        std::println("Audio: {} Hz, {} ch", _device_spec.freq, _device_spec.channels);

        for (int i = 0; i < SFX_CHANNELS; i++) {
            _sfx_streams[i] = SDL_CreateAudioStream(nullptr, &_device_spec);
            if (_sfx_streams[i]) {
                SDL_BindAudioStream(_device, _sfx_streams[i]);
                SDL_SetAudioStreamGain(_sfx_streams[i], sound_volume);
            }
        }

        _music_stream = SDL_CreateAudioStream(nullptr, &_device_spec);
        if (_music_stream) {
            SDL_BindAudioStream(_device, _music_stream);
            SDL_SetAudioStreamGain(_music_stream, music_volume);
        }

        _music_menu.load("music/menu.wav");
        _music_gameplay.load("music/gameplay.wav");

        _sfx_move.load("chess/move.wav");
        _sfx_capture.load("chess/capture.wav");
        _sfx_select.load("chess/select.wav");
        _sfx_check.load("chess/check.wav");
        _sfx_castle.load("chess/castle.wav");
        _sfx_promote.load("chess/promote.wav");
        _sfx_speed.load("chess/speed.wav");
        _sfx_start.load("chess/gamestart.wav");
        _sfx_victory.load("chess/victory.wav");

        _sfx_pu_teleport.load("chess/powerup_teleport.wav");
        _sfx_pu_bomb.load("chess/powerup_bomb.wav");
        _sfx_pu_freeze.load("chess/powerup_freeze.wav");
        _sfx_pu_promotion.load("chess/powerup_promotion.wav");
        _sfx_pu_spawn.load("chess/powerup_spawn.wav");
    }

    void destroy() {
        if (_music_stream) SDL_DestroyAudioStream(_music_stream);
        for (int i = 0; i < SFX_CHANNELS; i++) {
            if (_sfx_streams[i]) SDL_DestroyAudioStream(_sfx_streams[i]);
        }
        if (_device) SDL_CloseAudioDevice(_device);

        _music_menu.destroy(); _music_gameplay.destroy();
        _sfx_move.destroy(); _sfx_capture.destroy(); _sfx_select.destroy();
        _sfx_check.destroy(); _sfx_castle.destroy(); _sfx_promote.destroy();
        _sfx_speed.destroy(); _sfx_start.destroy(); _sfx_victory.destroy();
        _sfx_pu_teleport.destroy(); _sfx_pu_bomb.destroy();
        _sfx_pu_freeze.destroy(); _sfx_pu_promotion.destroy(); _sfx_pu_spawn.destroy();
    }

    void update() {
        if (!_current_music || !_music_stream) return;

        int queued = SDL_GetAudioStreamQueued(_music_stream);
        int threshold = _device_spec.freq * _device_spec.channels * sizeof(float);

        if (queued < threshold) {
            feed_music();
        }
    }

    void play_menu_music() {
        if (_current_music == &_music_menu) return;
        _current_music = &_music_menu;
        _music_pos = 0;
        if (_music_stream) SDL_ClearAudioStream(_music_stream);
        feed_music();
    }

    void play_gameplay_music() {
        if (_current_music == &_music_gameplay) return;
        _current_music = &_music_gameplay;
        _music_pos = 0;
        if (_music_stream) SDL_ClearAudioStream(_music_stream);
        feed_music();
    }

    void stop_music() {
        _current_music = nullptr;
        _music_pos = 0;
        if (_music_stream) SDL_ClearAudioStream(_music_stream);
    }

    void play_move()    { play_sfx(_sfx_move); }
    void play_capture() { play_sfx(_sfx_capture); }
    void play_select()  { play_sfx(_sfx_select); }
    void play_check()   { play_sfx(_sfx_check); }
    void play_castle()  { play_sfx(_sfx_castle); }
    void play_promote() { play_sfx(_sfx_promote); }
    void play_speed()   { play_sfx(_sfx_speed); }
    void play_start()   { play_sfx(_sfx_start); }
    void play_victory() { play_sfx(_sfx_victory); }

    void play_pu_teleport()  { play_sfx(_sfx_pu_teleport); }
    void play_pu_bomb()      { play_sfx(_sfx_pu_bomb); }
    void play_pu_freeze()    { play_sfx(_sfx_pu_freeze); }
    void play_pu_promotion() { play_sfx(_sfx_pu_promotion); }
    void play_pu_spawn()     { play_sfx(_sfx_pu_spawn); }

    float music_volume = 0.5f;
    float sound_volume = 0.5f;

    void adjust_music_volume(float delta) {
        music_volume = std::clamp(music_volume + delta, 0.0f, 1.0f);
        if (_music_stream) SDL_SetAudioStreamGain(_music_stream, music_volume);
    }

    void adjust_sound_volume(float delta) {
        sound_volume = std::clamp(sound_volume + delta, 0.0f, 1.0f);
        for (int i = 0; i < SFX_CHANNELS; i++) {
            if (_sfx_streams[i]) SDL_SetAudioStreamGain(_sfx_streams[i], sound_volume);
        }
    }

private:
    void feed_music() {
        if (!_current_music || !_current_music->buffer || !_music_stream) return;

        Uint32 chunk_size = _current_music->spec.freq * _current_music->spec.channels * sizeof(int16_t);
        Uint32 remaining = _current_music->buffer_size - _music_pos;

        if (remaining == 0) {
            _music_pos = 0;
            remaining = _current_music->buffer_size;
        }

        Uint32 feed = std::min(chunk_size, remaining);

        SDL_SetAudioStreamFormat(_music_stream, &_current_music->spec, &_device_spec);
        SDL_PutAudioStreamData(_music_stream, _current_music->buffer + _music_pos, feed);
        _music_pos += feed;
    }

    void play_sfx(AudioClip& clip) {
        if (!clip.buffer) return;

        int best = 0;
        int min_queued = INT_MAX;
        for (int i = 0; i < SFX_CHANNELS; i++) {
            if (!_sfx_streams[i]) continue;
            int q = SDL_GetAudioStreamQueued(_sfx_streams[i]);
            if (q < min_queued) { min_queued = q; best = i; }
        }

        if (!_sfx_streams[best]) return;

        SDL_SetAudioStreamFormat(_sfx_streams[best], &clip.spec, &_device_spec);
        SDL_PutAudioStreamData(_sfx_streams[best], clip.buffer, clip.buffer_size);
    }

    static constexpr int SFX_CHANNELS = 8;

    SDL_AudioDeviceID _device = 0;
    SDL_AudioStream* _music_stream = nullptr;
    SDL_AudioStream* _sfx_streams[SFX_CHANNELS] = {};
    SDL_AudioSpec _device_spec{};

    AudioClip* _current_music = nullptr;
    Uint32 _music_pos = 0;

    AudioClip _music_menu;
    AudioClip _music_gameplay;
    AudioClip _sfx_move, _sfx_capture, _sfx_select, _sfx_check;
    AudioClip _sfx_castle, _sfx_promote, _sfx_speed, _sfx_start, _sfx_victory;
    AudioClip _sfx_pu_teleport, _sfx_pu_bomb, _sfx_pu_freeze, _sfx_pu_promotion, _sfx_pu_spawn;
};
