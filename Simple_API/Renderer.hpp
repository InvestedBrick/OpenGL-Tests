#pragma once

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "GlProgram.hpp"


class Renderer{
public:
void clear() const;
void draw(const VertexArray& vao,const IndexBuffer& ibo,const GlProgram& program) const;  

};