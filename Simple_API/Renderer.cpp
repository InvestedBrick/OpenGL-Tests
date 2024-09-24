#include <iostream>
#include "Renderer.hpp"
#include "defines.hpp"

void Renderer::clear() const
{
    glCall(glClear(GL_COLOR_BUFFER_BIT)); 
}

void Renderer::draw(const VertexArray &vao, const IndexBuffer &ibo, const GlProgram &program,GLenum mode) const
{
    program.bind();
    vao.bind();
    std::cout << "----------" << std::endl;
    ibo.bind();


    glCall(glDrawElements(mode, ibo.get_count(),GL_UNSIGNED_INT, nullptr));


}
