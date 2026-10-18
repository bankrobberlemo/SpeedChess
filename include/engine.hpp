#pragma once
#include "time.hpp"
#include "input.hpp"
#include "window.hpp"
#include "chess/chess_game.hpp"

struct Engine {
    Engine() {
        _time.init();

        // create window and OpenGL context
        _window.init(1280, 720, 4);

        // initialize the chess game
        _game.init();
    }
    ~Engine() {
        _game.destroy();
        _window.destroy();
    }

    auto handle_sdl_event(SDL_Event& event) -> SDL_AppResult {
        switch (event.type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            default: break;
        }
        // toggle fullscreen with F11
        if (event.type == SDL_EventType::SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F11) {
            _fullscreen = !_fullscreen;
            SDL_SetWindowFullscreen(_window._window_p, _fullscreen);
        }
        Input::register_event(event);
        return SDL_AppResult::SDL_APP_CONTINUE;
    }
    auto handle_sdl_frame() -> SDL_AppResult {
        _time.update();
        Input::update_relative_state();

        // update game logic
        _game.handle_inputs();
        _game.update(_time._delta);

        // get current window size and update viewport
        int w, h;
        SDL_GetWindowSize(_window._window_p, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the chess game
        _game.draw(static_cast<float>(w), static_cast<float>(h));

        // present to screen
        SDL_GL_SwapWindow(_window._window_p);
        Input::flush();
        return SDL_AppResult::SDL_APP_CONTINUE;
    }

    Time _time;
    Window _window;
    ChessGame _game;
    bool _fullscreen = false;
};
