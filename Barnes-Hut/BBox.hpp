#pragma once
#include "../Simple_API/defines.hpp"
// Bounding box
class BBox{
public:
// xy determines bottom left
    float x;
    float y;
    float w;
    float h;

    BBox(float _x,float _y, float _w,float _h) : x(_x), y(_y), w(_w),h(_h){}

    inline float right() const noexcept{
        return x + w;
    }
    inline float top() const noexcept{
        return y + h;
    }
    inline float right() const noexcept{
        return x + w;
    }
    inline float right() const noexcept{
        return x + w;
    }
};