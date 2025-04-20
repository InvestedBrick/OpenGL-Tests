#pragma once

#include <GL/glew.h>

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define GRAVITATIONAL_CONSTANT 0.0015 // should be same as in compute.shader
#define MASS_TO_RADIUS 0.0001 // should be same as in compute.shader
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
