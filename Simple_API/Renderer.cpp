#include <iostream>
#include "Renderer.hpp"
#include "defines.hpp"

void Renderer::clear() const
{
    glCall(glClear(GL_COLOR_BUFFER_BIT)); 
}

void Renderer::draw(const VertexArray &vao, const IndexBuffer &ibo, const GlProgram &program) const
{
    program.bind();
    vao.bind();
    ibo.bind();

    glCall(glDrawElements(GL_TRIANGLES, ibo.get_count(),GL_UNSIGNED_INT, nullptr));


}
