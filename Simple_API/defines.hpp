#pragma once

#include <GL/glew.h>

#include <cassert>

typedef unsigned int uint;

#define glCall(x) GlClearError();\
    x;\
    assert(GlLogErrors());

void GlClearError();
bool GlLogErrors();
