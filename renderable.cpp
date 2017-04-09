/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "renderable.h"
#include <cassert>
#include <iostream>

Renderable::Renderable() : shown(true), element_array_buffer_id(0), program(NULL) {
    initializeOpenGLFunctions();
}

Renderable::~Renderable() {
    delete program;
}

void Renderable::show()
{
    this->shown = true;
}

void Renderable::hide()
{
    this->shown = false;
}

void Renderable::toggle_shown()
{
    this->shown = !this->shown;
}

void Renderable::compile(std::string vtx_shader_src, std::string frag_shader_src) {
    this->program->addShaderFromSourceCode(QOpenGLShader::Vertex, vtx_shader_src.c_str());
    this->program->addShaderFromSourceCode(QOpenGLShader::Fragment, frag_shader_src.c_str());
}

void Renderable::add_vertex_attrib(GLsizei bo_size, const void* ptr, GLenum access, int prog_id, std::string attrib_name, int size, GLenum type) {
    unsigned int buffer;    
    glGenBuffers(1, &buffer);   
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, bo_size, ptr, access);    
    int attrib_id = glGetAttribLocation(prog_id, attrib_name.c_str());
    Attribute attrib = Attribute(attrib_id, size, type, buffer);
    GLuint err = glGetError();    
    assert(err == 0);
    this->attribs[attrib_name] = attrib;
}

bool Renderable::has_element_array() const {
    return element_array_buffer_id != 0;
}

void Renderable::add_element_array(const std::vector<int>& elements) {
    if ( element_array_buffer_id != 0 ) {
        glDeleteBuffers(1, &element_array_buffer_id);
    }
    glGenBuffers(1, &element_array_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(elements[0]), &elements[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderable::bind() {    
    program->bind();
}

void Renderable::draw() {
    if (shown == false) {
        return ;
    }
    for ( auto i : this->attribs ) {
        auto a = i.second;
        glBindBuffer(GL_ARRAY_BUFFER, a.bo);
        glVertexAttribPointer(a.attrib_id, a.size, a.type, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(a.attrib_id);
    }
    if ( has_element_array() ) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->element_array_buffer_id);
    }
    this->draw2();
    if ( has_element_array() ) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    for ( auto i : this->attribs ) {
        auto a = i.second;
        glDisableVertexAttribArray(a.attrib_id);
    }
}
