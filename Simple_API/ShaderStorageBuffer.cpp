#include "ShaderStorageBuffer.hpp"
#include "defines.hpp"
ShaderStorageBuffer::ShaderStorageBuffer(const void *data, const unsigned int size)
{
    glCall(glGenBuffers(1, &m_Renderer_ID));
    glCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Renderer_ID));
    glCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
    glCall(glDeleteBuffers(1,&m_Renderer_ID));
}

void ShaderStorageBuffer::bind() const
{
    glCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Renderer_ID));
}

void ShaderStorageBuffer::bind_to(uint binding)
{
    glCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_Renderer_ID));
}

void ShaderStorageBuffer::unbind() const 
{
    glCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}
