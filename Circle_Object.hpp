#pragma once
#include "vec2.hpp"

struct Circle_Object
{
    Circle_Object(const float x, const float y,float m,const vec2f v = {0.f,0.f}) 
        : mass(m),pos(x,y), vel(v){}
    float mass;
    float padding_0;
    vec2f pos;
    vec2f vel;
    vec2f force{0.f,0.f};
};