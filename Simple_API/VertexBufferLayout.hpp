#pragma once
#include <vector>
#include <GL/glew.h>
#include <cassert>
#include "defines.hpp"
struct VertexBufferLayoutElement{
    unsigned int type;
    unsigned int count;
    bool normalised;

    static unsigned int get_size_of_type(unsigned int type){
        switch (type)
        {
            case GL_FLOAT:         return sizeof(float);
            case GL_UNSIGNED_INT:  return sizeof(unsigned int);
            case GL_UNSIGNED_BYTE: return sizeof(char);
        
        }
        assert(false && "Will not implement");
        return 0;
    }
};
class VertexBufferLayout{
private:
    std::vector<VertexBufferLayoutElement> m_elements;
    unsigned int m_stride;
public:
    VertexBufferLayout() : m_stride(0){}


    void push(unsigned int gl_type,unsigned int count);

    inline unsigned int get_stride() const{ return m_stride; }
    inline const std::vector<VertexBufferLayoutElement> get_elements() const {return m_elements;};
};