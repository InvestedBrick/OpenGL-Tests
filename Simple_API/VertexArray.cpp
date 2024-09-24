#include "VertexArray.hpp"
#include "defines.hpp"
#include <iostream>
VertexArray::VertexArray()
{
    glCall(glGenVertexArrays(1,&m_Renderer_ID));
}

VertexArray::~VertexArray()
{
    glCall(glDeleteVertexArrays(1,&m_Renderer_ID));
}

void VertexArray::add_buffer(const VertexBuffer &vb, const VertexBufferLayout &vbl)
{   
    bind();
    vb.bind();
    const auto& elements = vbl.get_elements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++){
        const auto& element = elements[i];
        glCall(glEnableVertexAttribArray(i));
        glCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, vbl.get_stride(), (const void*)offset));
        offset += element.count * VertexBufferLayoutElement::get_size_of_type(element.type);

    }
}
void VertexArray::bind() const
{
    std::cout << m_Renderer_ID << std::endl;
    glCall(glBindVertexArray(m_Renderer_ID));
    std::cout << "bound " << m_Renderer_ID << std::endl;
}

void VertexArray::unbind() const 
{
    glCall(glBindVertexArray(0));
}
