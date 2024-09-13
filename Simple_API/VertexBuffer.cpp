#include "VertexBuffer.hpp"
#include "defines.hpp"
VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
    glCall(glGenBuffers(1, &m_Renderer_ID));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Renderer_ID));
    glCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    glCall(glDeleteBuffers(1,&m_Renderer_ID));
}

void VertexBuffer::bind() const
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Renderer_ID));
}

void VertexBuffer::unbind() const 
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
