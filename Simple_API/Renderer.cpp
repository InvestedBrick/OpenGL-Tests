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
    ibo.bind();

    glCall(glDrawElements(mode, ibo.get_count(),GL_UNSIGNED_INT, nullptr));

}

void Renderer::draw_instanced(const VertexArray &vao,const IndexBuffer &ibo ,const GlProgram & program,GLenum mode, uint count) const
{
    program.bind();
    vao.bind();
    ibo.bind();

    glCall(glDrawElementsInstanced(mode, ibo.get_count(), GL_UNSIGNED_INT, nullptr,count));
}
