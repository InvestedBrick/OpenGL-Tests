#pragma once
#include <string>
#include <ostream>
struct vec2f{
    float x;
    float y;

    vec2f(float pos_x, float pos_y){
        x = pos_x;
        y = pos_y;
    }
    bool operator==(const vec2f& other) const{
        return x == other.x && y == other.y;
    }
    vec2f operator+(const vec2f& other) const{
        return vec2f(x + other.x,y + other.y);
    }
    vec2f& operator+=(const vec2f& other) {
        x += other.x;
        y += other.y;
        return *this; 
    }
    vec2f& operator-=(const vec2f& other) {
        x -= other.x;
        y -= other.y;
        return *this; 
    }
    vec2f operator-(const vec2f& other) const{
        return vec2f(x - other.x,y - other.y);
    }
    vec2f operator*(const float scalar) const{
        return vec2f(x * scalar,y * scalar);
    }
    vec2f operator/(const float scalar) const{
        return vec2f(x / scalar,y / scalar);
    }

    friend std::ostream& operator<<(std::ostream& os, const vec2f& vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }
};