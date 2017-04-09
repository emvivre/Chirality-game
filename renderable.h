/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef OBJECT_TO_RENDER_H
#define OBJECT_TO_RENDER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <string>
#include <iostream>
#include <vector>

template <class T> static GLenum deduce_opengl_type() { static_assert(false && sizeof(T), "Unable to deduce opengl type !"); return 0; } // ERROR
template <> GLenum deduce_opengl_type<float>() { return GL_FLOAT; }
template <> GLenum deduce_opengl_type<double>() { return GL_DOUBLE; }
template <> GLenum deduce_opengl_type<unsigned char>() { return GL_UNSIGNED_BYTE; }
template <> GLenum deduce_opengl_type<unsigned short>() { return GL_UNSIGNED_SHORT; }
template <> GLenum deduce_opengl_type<unsigned int>() { return GL_UNSIGNED_INT; }

// handle memory buffer -> GPU buffer
template <class T>
class ArrayBufferBinding : protected QOpenGLFunctions
{
    GLenum access;
    unsigned int nb_vector;
    unsigned int nb_elem_per_vector;
    T* buffer;    
    void init() {
        initializeOpenGLFunctions();
        glGenBuffers(1, &this->bo);
        this->type = deduce_opengl_type<T>();        
    }
public:
    unsigned int bo;
    GLenum type;
    ArrayBufferBinding(GLenum access, unsigned int nb_vector, unsigned int nb_elem_per_vector, T* buffer) : access(access), nb_vector(nb_vector), nb_elem_per_vector(nb_elem_per_vector), buffer(buffer) {
        init();
    }
    ~ArrayBufferBinding() {
        glDeleteBuffers(1, &this->bo);
    }
    unsigned int get_nb_vector() const { return nb_vector; }
    unsigned int get_nb_elem_per_vector() const { return nb_elem_per_vector; }
    unsigned int size() const { return nb_vector * nb_elem_per_vector; }
    GLenum get_type() const { return this->type; }
    unsigned int get_bo() const { return this->bo; }
    void refresh() {
        glBindBuffer(GL_ARRAY_BUFFER, this->bo);
        glBufferData(GL_ARRAY_BUFFER, size() * sizeof(T), this->buffer, this->access);
    }    
    T* operator[](int i) {
        return buffer + i * nb_elem_per_vector;        
    }
};

class QMouseEvent;

class Renderable : protected QOpenGLFunctions
{
public:
    struct Attribute {
        int attrib_id;
        unsigned int size;
        GLenum type;
        unsigned int bo;
        Attribute() {}
        Attribute(int attrib_id, unsigned int size, GLenum type, unsigned int bo) : attrib_id(attrib_id), size(size), type(type), bo(bo) {}
        friend std::ostream& operator<<(std::ostream& os, const Attribute& a) {
            return os << "attrib_id:" << a.attrib_id << " " <<
                         "size:" << a.size << " " <<
                         "type:0x" << std::hex << a.size << std::dec << " " <<
                         "bo:" << a.bo;
        }
    };
private:
    std::map<std::string, Attribute> attribs;
    virtual void draw2() = 0;
    bool shown;
    unsigned int element_array_buffer_id;
protected:
    QOpenGLShaderProgram* program;    
    Renderable();    
    void compile(std::string vtx_shader_src, std::string frag_shader_src);
    void add_vertex_attrib(GLsizei bo_size, const void* ptr, GLenum access, int prog_id, std::string attrib_name, int size, GLenum type);
    void add_element_array(const std::vector<int>& elements);
    bool has_element_array() const;

    template <class T>
    void add_vertex_attrib(std::string attrib_name, ArrayBufferBinding<T>* abb) {
        int attrib_id = glGetAttribLocation(this->program->programId(), attrib_name.c_str());
        Attribute attrib = Attribute(attrib_id, abb->get_nb_elem_per_vector(), abb->get_type(), abb->get_bo());
        this->attribs[attrib_name] = attrib;
    }
public:
    virtual ~Renderable();
    Attribute get_attrib(std::string name) const { return attribs.at(name); }
    void bind();
    void draw();
    void show();
    void hide();
    void toggle_shown();
    virtual void mousePressEvent(QMouseEvent*) {}

    template <class T> void set_uniform(std::string name, T v) {
        program->setUniformValue(name.c_str(), v);
    }   
};

#endif // OBJECT_TO_RENDER_H
