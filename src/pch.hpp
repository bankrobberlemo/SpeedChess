#include <print>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <fstream>
#include <iterator>
#include <chrono>
#include <algorithm>
#include <cmath>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h> // main header
#include <SDL3/SDL_main.h> // entry point
#include <SDL3_image/SDL_image.h> // image loading
#include <SDL3/SDL_video.h> 

#include <glbinding/gl/gl.h> // opengl headers
#include <glbinding/glbinding.h> // main header
#include <glbinding-aux/debug.h> // utils
using namespace gl;

#include <glm/glm.hpp> // main header
#include <glm/ext/matrix_transform.hpp> // translation, rotation, etc
#include <glm/ext/matrix_clip_space.hpp> // orthographic and perspective projections
#include <glm/gtc/type_ptr.hpp> // obtain pointers

