#pragma once

#include <GL/glew.h>

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1080
#define GRAVITATIONAL_CONSTANT 0.01
#define MASS_TO_RADIUS 0.0001
#define MOUSE_NOTHING_POS -10
#define VSYNC 1
#define CPU_STATE 1
#define GPU_STATE 0
#define CIRCLE_SEGMENTS 10
#include <cassert>

typedef unsigned int uint;

#define glCall(x) GlClearError();\
    x;\
    assert(GlLogErrors());

void GlClearError();
bool GlLogErrors();
