#pragma once
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
class VertexArray{
private:
    unsigned int m_Renderer_ID;
public:

    VertexArray();
    ~VertexArray();

    void add_buffer(const VertexBuffer& vb,const VertexBufferLayout& vbl);
    void bind() const ;
    void unbind() const ;
};