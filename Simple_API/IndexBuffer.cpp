#include "IndexBuffer.hpp"
#include "Renderer.hpp"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_count(count)
{
    glCall(glGenBuffers(1, &m_Renderer_ID));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Renderer_ID));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    glCall(glDeleteBuffers(1,&m_Renderer_ID));
}

void IndexBuffer::bind() const 
{
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Renderer_ID));
}

void IndexBuffer::unbind() const
{
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
