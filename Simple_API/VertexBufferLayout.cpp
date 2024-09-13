#include "VertexBufferLayout.hpp"
void VertexBufferLayout::push(unsigned int gl_type, unsigned int count)
{
    m_elements.push_back({gl_type,count,gl_type == GL_UNSIGNED_BYTE});
    m_stride += count * VertexBufferLayoutElement::get_size_of_type(gl_type);
}