#pragma once

typedef unsigned int uint;
#include <GL/glew.h>
#include <cassert>

#define glCall(x) GlClearError();\
    x;\
    assert(GlLogErrors());

void GlClearError();
bool GlLogErrors();
