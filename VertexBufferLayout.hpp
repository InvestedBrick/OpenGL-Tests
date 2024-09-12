#pragma once
#include <vector>
#include <GL/glew.h>
#include <cassert>
#include "Renderer.hpp"
struct VertexBufferLayoutElement{
    unsigned int type;
    unsigned int count;
    bool normalised;

    static unsigned int get_size_of_type(unsigned int type){
        switch (type)
        {
            case GL_FLOAT:         return 4;
            case GL_UNSIGNED_INT:  return 4;
            case GL_UNSIGNED_BYTE: return 1;
        
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

    template<typename T>
    void push(unsigned int count){
        static_assert(false);
    }

    void push(unsigned int gl_type,unsigned int count);

    inline unsigned int get_stride() const{ return m_stride; }
    inline const std::vector<VertexBufferLayoutElement> get_elements() const {return m_elements;};
};