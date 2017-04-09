/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "glwidget.h"
#include "renderable.h"
#include <QTimer>
#include <QMouseEvent>
#include "ply_binary_reader.h"
#include <map>
#include "img_question.h"
#include "img_button_no.h"
#include "img_button_yes.h"
#include "img_button_prev.h"
#include "img_button_next.h"
#include "img_button_prev_gray.h"
#include "img_button_next_gray.h"
#include "molecule_ethanol.h"
#include "molecule_methane.h"
#include "molecule_R____alanine.h"
#include "molecule_R____asparagine.h"
#include "molecule_R____limonene.h"
#include "molecule_R____thalidomide.h"
#include "molecule_S____alanine.h"
#include "molecule_S____asparagine.h"
#include "molecule_S____limonene.h"
#include "molecule_S____thalidomide.h"
#include "img_molecule_alanine.h"
#include "img_molecule_asparagine.h"
#include "img_molecule_ethanol.h"
#include "img_molecule_limonene.h"
#include "img_molecule_methane.h"
#include "img_molecule_thalidomide.h"
#include "img_1_over_6.h"
#include "img_2_over_6.h"
#include "img_3_over_6.h"
#include "img_4_over_6.h"
#include "img_5_over_6.h"
#include "img_6_over_6.h"
#include "img_white.h"
#include <ctime>
#include "img_about.h"
#include "img_question_mark.h"
#include "img_game_over.h"
#include "img_button_reset.h"
#include "source_code.h"
#include <QFileDialog>

static const int NB_OBJECTS = 6;
static const float OBJECT_VIEW_HEIGHT_PERCENT = 0.8;
static const float BUTTON_PREV_NEXT_HEIGHT_PERCENT = 0.2;

static std::pair<const void*, unsigned int> molecules_left[] {    
    { molecule_methane, molecule_methane_sz },
    { molecule_ethanol, molecule_ethanol_sz },
    { molecule_S____alanine, molecule_S____alanine_sz },
    { molecule_S____limonene, molecule_S____limonene_sz },
    { molecule_S____asparagine, molecule_S____asparagine_sz },
    { molecule_R____thalidomide, molecule_R____thalidomide_sz },
};

static std::pair<const void*, unsigned int> molecules_right[] {
    { molecule_methane, molecule_methane_sz },
    { molecule_ethanol, molecule_ethanol_sz },    
    { molecule_R____alanine, molecule_R____alanine_sz },
    { molecule_R____limonene, molecule_R____limonene_sz },
    { molecule_R____asparagine, molecule_R____asparagine_sz },
    { molecule_S____thalidomide, molecule_S____thalidomide_sz },
};

struct Texture {
    const unsigned char* data;
    unsigned int w;
    unsigned int h;
    Texture(const unsigned char* data, unsigned int w, unsigned int h) : data(data), w(w), h(h) {}
};

static std::vector<Texture> molecules_img = {
    Texture(img_molecule_methane, img_molecule_methane_width, img_molecule_methane_height),
    Texture(img_molecule_ethanol, img_molecule_ethanol_width, img_molecule_ethanol_height),
    Texture(img_molecule_alanine, img_molecule_alanine_width, img_molecule_alanine_height),
    Texture(img_molecule_limonene, img_molecule_limonene_width, img_molecule_limonene_height),
    Texture(img_molecule_asparagine, img_molecule_asparagine_width, img_molecule_asparagine_height),
    Texture(img_molecule_thalidomide, img_molecule_thalidomide_width, img_molecule_thalidomide_height),
};

static std::vector<Texture> level_img = {
    Texture(img_1_over_6, img_1_over_6_width, img_1_over_6_height),
    Texture(img_2_over_6, img_2_over_6_width, img_2_over_6_height),
    Texture(img_3_over_6, img_3_over_6_width, img_3_over_6_height),
    Texture(img_4_over_6, img_4_over_6_width, img_4_over_6_height),
    Texture(img_5_over_6, img_5_over_6_width, img_5_over_6_height),
    Texture(img_6_over_6, img_6_over_6_width, img_6_over_6_height),
};

static std::vector<QVector3D> molecule_translate = {
    QVector3D(0, 0, 0),
    QVector3D(0, 0, 0),
    QVector3D(0, 0, 0),
    QVector3D(0, 0, -2),
    QVector3D(0, 0, 0),
    QVector3D(0, 0, -4),
};

static bool molecule_answer[] {
    false,
    false,
    true,
    true,
    true,
    true,
};

static std::pair<QQuaternion, QQuaternion> new_quaternion_view() {
    QQuaternion ql, qr;
    qr *= QQuaternion::fromAxisAndAngle(qr.conjugate().rotatedVector(QVector3D(0,1,0)), 180);
    return std::make_pair(ql, qr);
}

class SignalGenerator
{
    bool is_pause;
protected:
    float init;
    float current;
    virtual float next() = 0;
    virtual void reinitialize2() {}
public:
    SignalGenerator(float init) : is_pause(true), init(init), current(init) {}
    void pause() { is_pause = true; }
    void unpause() { is_pause = false; }
    virtual float operator()() {
        if ( is_pause == true ) return current;
        return next();
    }
    void reinitialize() {
        current = init;
        reinitialize2();
    }
    float get_current() const { return current; }
};

class VibrationGeneration : public SignalGenerator
{
    float min, max;
    int nb_time;
    int nb_time_max;
    bool is_grow_up;
    float step;
public:
    VibrationGeneration(float init, float min, float max, int nb_time_max, float step=1) : SignalGenerator(init), min(min), max(max), nb_time(0), nb_time_max(nb_time_max), is_grow_up(true), step(step) {}
    void reinitialize2() {
        nb_time = 0;
    }
    float next() {
        if (nb_time == nb_time_max ) return init;
        if (is_grow_up) {
            current += step;
            if ( current > max ) {
                current = max;
                is_grow_up = !is_grow_up;
                nb_time++;
            }
        }
        else {
            current -= step;
            if ( current < min) {
                current = min;
                is_grow_up = !is_grow_up;
                nb_time++;
            }
        }
        return current;
    }
};

class LinearGenerator : public SignalGenerator
{
    float min, max;
    float step;
    bool is_grow_up;
public:
    LinearGenerator(float current, float min, float max, float step=1, bool is_grow_up=true) : SignalGenerator(current), min(min), max(max), step(step), is_grow_up(is_grow_up) {}
    float next() {
        if (is_grow_up) {
            current += step;
            if ( current > max ) {
                current = max;
                is_grow_up = !is_grow_up;
            }
        }
        else {
            current -= step;
            if ( current < min) {
                current = min;
                is_grow_up = !is_grow_up;
            }
        }
        return current;
    }
};

class LinearDownGenerator : public SignalGenerator
{
    float min;
    float step;
public:
    LinearDownGenerator(float init, float min, float step=1) : SignalGenerator(init), min(min), step(step) {}
    float next() {
        current -= step;
        if ( current < min)
            current = min;
        return current;
    }
};

class MeshShader : public Renderable
{
    static constexpr const char* vtx_shader_src =
           "#version 120\n"
           "attribute highp vec3 col_i;\n"
           "attribute highp vec4 pos_i;\n"
           "attribute highp vec4 normal_i;\n"
           "uniform highp mat4 mtx_u;\n"
           "varying highp vec4 col_o;\n"
           "\n"
           "void main(){\n"
           "    gl_Position = mtx_u * pos_i;\n"
           "    vec4 l = vec4(0,0,1,1);\n"
           "    float p = max(0.3, abs( dot(normal_i,l)));\n"
           "    col_o = vec4(col_i*p,1);\n"
           "}";
    static constexpr const char* frag_shader_src =
           "varying highp vec4 col_o;\n"
           "void main() {\n"
           "    gl_FragColor = col_o;\n"
           "}\n";

     int nb_elements;
public:
     MeshShader(const std::vector<QVector3D>& coord, const std::vector<int>& triangles, const std::vector<QVector3D>& colors, const std::vector<QVector3D>& normals) {
         this->program = new QOpenGLShaderProgram();
         compile(vtx_shader_src, frag_shader_src);
         this->program->link();
         program->bind();
         this->add_vertex_attrib(sizeof(coord[0]) * coord.size(), &coord[0], GL_STATIC_DRAW, this->program->programId(), "pos_i", 3, GL_FLOAT);
         this->add_vertex_attrib(sizeof(coord[0]) * colors.size(), &colors[0], GL_STATIC_DRAW, this->program->programId(), "col_i", 3, GL_FLOAT);
         this->add_vertex_attrib(sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW, this->program->programId(), "normal_i", 3, GL_FLOAT);
         this->add_element_array(triangles);
         this->nb_elements = triangles.size();
     }

     void draw2() {
         glDrawElements(GL_TRIANGLES, nb_elements, GL_UNSIGNED_INT, 0);
     }
};

class ArrowShader : public Renderable
{
    static constexpr const char* vtx_shader_src =
            "#version 120\n"
            "attribute highp vec4 pos_a;\n"
            "uniform lowp vec4 col_a;\n"
            "varying lowp vec4 col;\n"
            "uniform highp mat4 mtx_u;\n"
            "void main() {\n"
            "    col = col_a;\n"
            "    gl_Position = mtx_u * pos_a;\n"
            "}\n";
    static constexpr const char* frag_shader_src =
            "precision lowp float;\n"
            "varying lowp vec4 col;\n"
            "void main() {\n"
            "    gl_FragColor = col;\n"
            "}\n";
    const int NB_LINES = 4;
    const QVector2D& pos_begin, &pos_end;
    static const int NB_POINTS = 2;
    QVector2D pos[NB_POINTS];
    ArrayBufferBinding<float>* abb_pos;
public:
    ArrowShader(const QVector2D& pos_begin, const QVector2D &pos_end) : pos_begin(pos_begin), pos_end(pos_end) {
        this->program = new QOpenGLShaderProgram();
        this->compile(vtx_shader_src, frag_shader_src);
        this->program->link();
        this->add_vertex_attrib(sizeof this->pos, this->pos, GL_STATIC_DRAW, this->program->programId(), "pos_a", 2, GL_FLOAT);
        abb_pos = new ArrayBufferBinding<float>(GL_DYNAMIC_DRAW, NB_POINTS, 2, &this->pos[0][0]);
        this->add_vertex_attrib("pos_a", abb_pos);
    }
    void draw2() {
        this->pos[0] = pos_begin;
        this->pos[1] = pos_end;
        this->abb_pos->refresh();
        glLineWidth(10);
        glDrawArrays(GL_LINES, 0, NB_POINTS*2);
    }
};

class BorderToRender : public Renderable
{
    static constexpr const char* vtx_shader_src =
            "#version 120\n"
            "attribute highp vec4 pos_a;\n"
            "uniform lowp vec4 col_a;\n"
            "varying lowp vec4 col;\n"
            "uniform highp mat4 mtx_u;\n"
            "void main() {\n"
            "    col = col_a;\n"
            "    gl_Position = mtx_u * pos_a;\n"
            "}\n";
    static constexpr const char* frag_shader_src =
            "varying lowp vec4 col;\n"
            "void main() {\n"
            "    gl_FragColor = col;\n"
            "}\n";
    const int NB_LINES = 4;
    const float pos[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1} };
public:
    BorderToRender() {
        this->program = new QOpenGLShaderProgram();
        this->compile(vtx_shader_src, frag_shader_src);
        this->program->link();
        this->add_vertex_attrib(sizeof this->pos, this->pos, GL_STATIC_DRAW, this->program->programId(), "pos_a", 2, GL_FLOAT);
    }
    void draw2() {
        glLineWidth(20);
        glDrawArrays(GL_LINE_LOOP, 0, 4*2);
    }
};

class UniformColorToRender : public Renderable
{
    static constexpr const char* vtx_shader_src =
            "#version 120\n"
            "attribute highp vec4 pos_a;\n"
            "uniform lowp vec4 col_a;\n"
            "varying lowp vec4 col;\n"
            "uniform highp mat4 mtx_u;\n"
            "void main() {\n"
            "    col = col_a;\n"
            "    gl_Position = mtx_u * pos_a;\n"
            "}\n";
    static constexpr const char* frag_shader_src =
            "varying lowp vec4 col;\n"
            "void main() {\n"
            "    gl_FragColor = col;\n"
            "}\n";
    const int NB_VERTICES = 4;
    const float pos[4][2] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } };
public:
    UniformColorToRender() {
        this->program = new QOpenGLShaderProgram();
        this->compile(vtx_shader_src, frag_shader_src);
        this->program->link();
        this->add_vertex_attrib(sizeof this->pos, this->pos, GL_STATIC_DRAW, this->program->programId(), "pos_a", 2, GL_FLOAT);
    }
    void draw2() {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4*2);
    }
};

class CubeToRender : public Renderable
{
    static constexpr const char* vtx_shader_src =
            "#version 120\n"
            "attribute highp vec4 pos_a;\n"
            "attribute lowp vec4 col_a;\n"
            "varying lowp vec4 col;\n"
            "uniform highp mat4 mtx_u;\n"
            "void main() {\n"
            "    col = col_a;\n"
            "    gl_Position = mtx_u * pos_a;\n"
            "}\n";
    static constexpr const char* frag_shader_src =
            "varying lowp vec4 col;\n"
            "void main() {\n"
            "    gl_FragColor = col;\n"
            "}\n";
    const float pos[8][3] = {
            {-1, -1,  1},
            { 1, -1,  1},
            { 1,  1,  1},
            {-1,  1,  1},
            { 1, -1, -1},
            { 1,  1, -1},
            {-1,  1, -1},
            {-1, -1, -1},
    };
    const float col[8][3] = {
            { 1, 0,     0 },
            { 0, 1,     0 },
            { 0, 0,     1 },
            { 0, 0.5, 0.5 },
            { 0.5, 0.5, 0.5 },
            { 0.5, 0, 0.5 },
            { 1, 1, 0 },
            { 1, 0, 1 },
    };
    const unsigned int triangles_index[12][3] = {
        { 0, 1, 2 },
        { 0, 2, 3 },
        {1, 4, 5},
        {1, 5, 2},
        {2, 5, 6},
        {2, 6, 3},
        {0, 7, 4},
        {0, 4, 1},
        {0, 3, 6},
        {0, 6, 7},
        {7, 6, 5},
        {7, 5, 4},
    };
    const int NB_TRIANGLES = 12;
public:
    CubeToRender() {
        this->program = new QOpenGLShaderProgram();
        this->compile(vtx_shader_src, frag_shader_src);
        this->program->link();
        this->add_vertex_attrib(sizeof this->pos, this->pos, GL_STATIC_DRAW, this->program->programId(), "pos_a", 3, GL_FLOAT);
        this->add_vertex_attrib(sizeof this->col, this->col, GL_STATIC_DRAW, this->program->programId(), "col_a", 3, GL_FLOAT);
    }
    void draw2() {
        glDrawElements(GL_TRIANGLES, NB_TRIANGLES*3, GL_UNSIGNED_INT, triangles_index);
    }
};

class RectangleTextured : public Renderable
{
    static constexpr const char* vtx_shader_src =
            "#version 120\n"
            "attribute highp vec4 pos_a;\n"
            "attribute highp vec2 uv_a;\n"
            "varying lowp vec2 uv;\n"
            "uniform highp mat4 mtx_u;\n"
            "void main() {\n"
            "    uv = uv_a;\n"
            "    gl_Position = mtx_u * pos_a;\n"
            "}\n";
    static constexpr const char* frag_shader_src =
            "varying lowp vec2 uv;\n"
            "uniform sampler2D sampler;"
            "void main() {\n"
            "    gl_FragColor = texture2D(sampler, uv);\n"
            "}\n";
    const int NB_VERTICES = 4;
    const float pos[4][2] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } };
    const float uv[4][2] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } };
    unsigned int texture_id;
    unsigned int pbo_texture;
public:
    RectangleTextured(unsigned int w, unsigned int h, const unsigned char* data) {
        glGenTextures(1, &this->texture_id);
        glBindTexture(GL_TEXTURE_2D, this->texture_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        this->program = new QOpenGLShaderProgram();
        this->compile(vtx_shader_src, frag_shader_src);
        this->program->link();
        this->add_vertex_attrib(sizeof this->pos, this->pos, GL_STATIC_DRAW, this->program->programId(), "pos_a", 2, GL_FLOAT);
        this->add_vertex_attrib(sizeof this->uv, this->uv, GL_STATIC_DRAW, this->program->programId(), "uv_a", 2, GL_FLOAT);
    }
    void draw2() {
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture_id);
        glUniform1i(glGetUniformLocation(this->program->programId(), "sampler"), 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4*2);
    }
};

GLWidget::GLWidget(QGLWidget *parent) : QGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

int GLWidget::y_to_object_i(int y)
{
    int h_step = this->screen_height / NB_OBJECTS;
    int object_i = y / h_step;
    return NB_OBJECTS - 1 - object_i;
}

void GLWidget::update_view_rotation_left_right()
{
    this->view_rotate_left = this->rotation_saved[this->object_current].first;
    this->view_rotate_right = this->rotation_saved[this->object_current].second;
}


static std::pair<QQuaternion, QQuaternion> get_init_view(int i) {
    std::pair<QQuaternion, QQuaternion> p = new_quaternion_view();
    switch(i) {
    case 2:
        p.second = QQuaternion(0.453723, 0.320716, 0.0927651, 0.82627);
        break;
    case 4:
        p.first = QQuaternion(-0.38644, 0.536061, -0.0611206, -0.748054);
        p.second = QQuaternion(-0.0446637, 0.801351, 0.558901, -0.208527);
        break;
    case 5:
        p.second = QQuaternion(-0.969009, -0.0426268, -0.0432263, 0.239504);
        break;
    default:
        break;
    }
    return p;
}

void GLWidget::reset_game() {
    this->current_level_max = 0;
    this->show_about = false;
    this->show_game_over = false;
    object_current = 0;
    mouseLastPointInitialized = false;
    for ( unsigned int i = 0; i < this->rotation_saved.size(); i++ ) {
        this->rotation_saved[i] = get_init_view(i);
    }   
    update_view_rotation_left_right();
}

void GLWidget::mousePressEvent(QMouseEvent *me)
{       
    if ( this->show_game_over == true ) {
        this->show_game_over = false;
        return ;
    }
    if ( this->show_about == true ) {
        if ( me->y() > 0.6 * this->screen_height && (me->x() > this->screen_width / 3. && me->x() < this->screen_width * 2/3.)) {
            QString filename = QFileDialog::getSaveFileName(this, "Export du Code Source", "", ".tar.bz2");
            if (filename.length() == 0) {
                return ;
            }
            QFile f(filename);
            f.remove();
            f.open(QIODevice::WriteOnly);
            f.write((const char*)source_code, source_code_sz);
        }
        this->show_about = false ;
        return ;
    }
    if ( me->y() > int(this->screen_height - img_button_yes_height) && me->x() > this->screen_width * 6 / 7. ) {
        this->show_about = true;
        return ;
    }
    if (me->y() > 0.8*this->screen_height) {
        if ( me->x() < screen_width/8 ) {
            reset_game();
            return ;
        }
        bool is_click_on_yes = me->x() < this->screen_width/2;
        if ( is_click_on_yes == molecule_answer[object_current]) {
            this->rotation_saved[this->object_current] = std::make_pair(this->view_rotate_left, this->view_rotate_right);
            if ( object_current < NB_OBJECTS-1) {
                object_current = object_current+1;
            } else {
                this->show_game_over = true;
            }
            if ( object_current > this->current_level_max) {
                this->current_level_max = object_current;
            }
            update_view_rotation_left_right();
        } else {
            this->offset_sg->reinitialize();
            this->offset_sg->unpause();
        }
    } else if ( me->y() < this->screen_height*(BUTTON_PREV_NEXT_HEIGHT_PERCENT)) {
        if ( me->x() < this->screen_width/4. || me->x() > this->screen_width * 3/4. ) {
            // prev / next button
            bool is_click_on_prev = me->x() < this->screen_width/2;
            int nb_object_known = this->current_level_max+1;
            if ( nb_object_known > 1 ) {
                this->rotation_saved[this->object_current] = std::make_pair(this->view_rotate_left, this->view_rotate_right);
                if ( is_click_on_prev ) {
                    if ( this->object_current > 0 ) {
                        this->object_current--;
                    }
                } else {
                    if ( this->object_current < this->current_level_max ){
                        this->object_current++;
                    }
                }
                update_view_rotation_left_right();
            }
        } else {
            // reset view
            std::pair<QQuaternion, QQuaternion> pview = get_init_view(this->object_current);
            this->view_rotate_left = pview.first;
            this->view_rotate_right = pview.second;
        }
    }
}

void GLWidget::rotate_view(QVector2D v)
{
    const float ROT_SCALE = 1;
    this->mouseLastPoint += QPoint(v.x(), v.y());
    float lx = QVector2D::dotProduct(v, QVector2D(1,0));
    float ly = QVector2D::dotProduct(v, QVector2D(0,1));
    QQuaternion& view_rot = is_view_rotate_left ? this->view_rotate_left : this->view_rotate_right;
    view_rot *= QQuaternion::fromAxisAndAngle(view_rot.conjugate().rotatedVector(QVector3D(1,0,0)), ROT_SCALE*ly);
    view_rot *= QQuaternion::fromAxisAndAngle(view_rot.conjugate().rotatedVector(QVector3D(0,1,0)), ROT_SCALE*lx);
}

void GLWidget::mouseMoveEvent(QMouseEvent *me) {
    if ( me->buttons() == Qt::NoButton || me->y() < this->screen_height * BUTTON_PREV_NEXT_HEIGHT_PERCENT || me->y() > this->screen_height * (0.5+BUTTON_PREV_NEXT_HEIGHT_PERCENT) ) {
        return ;
    }
    if ( mouseLastPointInitialized == false ) {
        this->mouseLastPoint = me->pos();
        this->mouseLastPointInitialized = true;
        is_view_rotate_left = me->pos().x() < this->screen_width/2;
    }
    QVector2D v = QVector2D(me->pos() - mouseLastPoint);
    if (v.length() > 10 && v.length() < 40 ) {
        this->last_view_rot_dir = v;
        this->view_rot_coef_sg->reinitialize();
        this->view_rot_coef_sg->unpause();
    } else {
        this->last_view_rot_dir = QVector2D(0,0);
    }
    rotate_view(v);
    this->mouseLastPoint = me->pos();
    updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent*)
{    
    this->mouseLastPointInitialized = false;
    updateGL();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    for ( int i = 0; i < NB_OBJECTS; i++ ) {
        {
            std::vector<QVector3D> coord;
            std::vector<QVector3D> colors;
            std::vector<int> triangles;
            std::vector<QVector3D> normals;
            PlyBinaryReader::read(std::string((const char*)molecules_left[i].first, molecules_left[i].second), coord, triangles, colors, normals);
            Renderable* r = new MeshShader(coord, triangles, colors, normals);
            this->objects_to_render_left.push_back(r);
        }
        {
            std::vector<QVector3D> coord;
            std::vector<QVector3D> colors;
            std::vector<int> triangles;
            std::vector<QVector3D> normals;
            PlyBinaryReader::read(std::string((const char*)molecules_right[i].first, molecules_right[i].second), coord, triangles, colors, normals);
            Renderable* r = new MeshShader(coord, triangles, colors, normals);
            this->objects_to_render_right.push_back(r);
        }
    }
    for ( const auto& t : ::molecules_img ) {
        Renderable* r = new RectangleTextured(t.w, t.h, t.data);
        this->molecule_img_shader.push_back(r);
    }
    for ( const auto& t : ::level_img ) {
        Renderable* r = new RectangleTextured(t.w, t.h, t.data);
        this->level_img_shader.push_back(r);
    }
    this->white_shader = new RectangleTextured(img_white_width, img_white_height, img_white);
    this->button_shader_prev = new RectangleTextured(img_button_prev_width, img_button_prev_height, img_button_prev);
    this->button_shader_next = new RectangleTextured(img_button_next_width, img_button_next_height, img_button_next);
    this->button_shader_prev_gray = new RectangleTextured(img_button_prev_gray_width, img_button_prev_gray_height, img_button_prev_gray);
    this->button_shader_next_gray = new RectangleTextured(img_button_next_gray_width, img_button_next_gray_height, img_button_next_gray);
    this->button_shader_yes = new RectangleTextured(img_button_yes_width, img_button_yes_height, img_button_yes);
    this->button_shader_no = new RectangleTextured(img_button_no_width, img_button_no_height, img_button_no);
    this->button_shader_reset = new RectangleTextured(img_button_reset_width, img_button_reset_height, img_button_reset);
    this->border_shader = new BorderToRender();
    this->about_shader = new RectangleTextured(img_about_width, img_about_height, img_about);
    this->question_mark_shader = new RectangleTextured(img_question_mark_width, img_question_mark_height, img_question_mark);
    this->game_over_shader = new RectangleTextured(img_game_over_width, img_game_over_height, img_game_over);
    this->fov = 90;

    this->offset_sg = new VibrationGeneration(0, -10, 10, 10, 8);
    this->view_rot_coef_sg = new LinearDownGenerator(1, 0, 0.01);

    for ( int i = 0; i < NB_OBJECTS; i++ ) {
        this->rotation_saved.push_back(new_quaternion_view());
    }
    reset_game();

    QTimer* timer= new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(30);    

    glClearColor(0x49/255., 0x93/255., 0xd4/255., 1);

}

void GLWidget::resizeGL(int w, int h)
{
    if (w == 0) { w = 1; }
    if (h == 0) { h = 1; }
    this->screen_width = w;
    this->screen_height = h;    
    projection = QMatrix4x4();
    projection.perspective(this->fov, (w/2.)*1./h, 0.01, 100.0);
    projection_orth = QMatrix4x4();
    projection_orth.ortho(0, 1, 1, 0, 0, 100.0);
}

void GLWidget::paintGL()
{
    int x_off = (*this->offset_sg)();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 matrix;
    matrix.translate(0, 0, -6);
    matrix.translate(::molecule_translate[this->object_current]);

    // draw object left    
    glViewport(x_off+0, 0, this->screen_width/2., this->screen_height);
    Renderable* o = objects_to_render_left[this->object_current];
    o->bind();
    QMatrix4x4 m = matrix;
    m.rotate(view_rotate_left);
    o->set_uniform("mtx_u", this->projection * m);
    o->draw();

    // draw object right
    glViewport(x_off+this->screen_width/2., 0, this->screen_width/2., this->screen_height);
    matrix.rotate(view_rotate_right);    
    o = objects_to_render_right[this->object_current];
    o->bind();
    o->set_uniform("mtx_u", this->projection * matrix);
    o->draw();

    // draw white strip
    const int button_h = this->screen_height*BUTTON_PREV_NEXT_HEIGHT_PERCENT;
    const int button_w = button_h;
    {
        int x = x_off+button_w/2;
        int y = this->screen_height - button_h *3/4.;
        int w = this->screen_width - button_w;
        int h = button_h/2;
        glViewport(x_off + x, y, w, h);
        o = this->white_shader;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        glDisable(GL_DEPTH_TEST);
        o->draw();
        glEnable(GL_DEPTH_TEST);
    }

    // draw message
    {
        float tbr = img_question_height * 1./ img_question_width;
        int tbw = this->screen_width - 2*button_w;
        int tbh = tbw * tbr;
        int tbx = button_w;
        int tby = this->screen_height - button_h + (button_h - tbh)/2;
        glViewport(x_off+tbx, tby, tbw, tbh);
        o = this->molecule_img_shader[this->object_current];
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        o->draw();
    }

    // draw button left prev    
    glViewport(x_off+0, this->screen_height-button_h, button_h, button_h);    
    o = (this->object_current > 0) ? this->button_shader_prev : this->button_shader_prev_gray;
    o->bind();
    o->set_uniform("mtx_u", this->projection_orth);
    o->draw();

    // draw button right next
    glViewport(x_off+this->screen_width-button_h, this->screen_height-button_h, button_h, button_h);
    o = (this->object_current < this->current_level_max) ? this->button_shader_next : this->button_shader_next_gray;
    o->bind();
    o->set_uniform("mtx_u", this->projection_orth);
    o->draw();

    // draw button white strip
    {
        int x = 0;
        int y = 0;
        int w = this->screen_width;
        int h = img_button_yes_height;
        glViewport(x_off + x, y, w, h);
        o = this->white_shader;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        glDisable(GL_DEPTH_TEST);
        o->draw();
        glEnable(GL_DEPTH_TEST);
    }

    {
        // draw button left yes
        int button_w = screen_width/4;
        int button_h = img_button_yes_height;
        int x = this->screen_width/8;
        int y = 0;
        glViewport(x_off+x, y, button_w, button_h);
        o = this->button_shader_yes;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        o->draw();

        // draw button right no
        glViewport(x_off+this->screen_width*3./4 - button_w/2, 0, button_w, button_h);
        o = this->button_shader_no;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        o->draw();
    }

    // draw level number
    {
        int level_msg_w = this->screen_width/7.;
        float r = ::level_img[this->object_current].h * 1. / ::level_img[this->object_current].w;
        int level_msg_h = r * level_msg_w;
        int level_msg_x = this->screen_width*3/7.;
        int level_msg_y = (img_button_yes_height - level_msg_h)/2;
        glViewport(x_off+level_msg_x, level_msg_y, level_msg_w, level_msg_h);
        o = this->level_img_shader[this->object_current];
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        o->draw();
    }

    // draw vertical white strip
    {
        int w = 6;
        int h = this->screen_height / 2;
        int x = screen_width/2 - w/2;
        int y = this->screen_height * 1/4.;
        glViewport(x_off + x, y, w, h);
        o = this->white_shader;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        glDisable(GL_DEPTH_TEST);
        o->draw();
        glEnable(GL_DEPTH_TEST);
    }

    // draw question mark
    {
        int h = img_button_yes_height*3/4.;
        int w = h * img_question_mark_width * 1. / img_question_mark_height;
        int x = this->screen_width - 2 * w;
        int y = 10;
        glViewport(x_off + x, y, w, h);
        o = this->question_mark_shader;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        o->draw();
    }

    // draw reset image
    {
        int h = img_button_yes_height*3/4.;
        int w = h * img_button_reset_width * 1. / img_button_reset_height;
        int x = 10;
        int y = 10;
        glViewport(x_off + x, y, w, h);
        o = this->button_shader_reset;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        glDisable(GL_DEPTH_TEST);
        o->draw();
        glEnable(GL_DEPTH_TEST);
    }

    // display about image
    if ( this->show_about ) {
        int h = this->screen_height;
        int w = h * img_about_width * 1./ img_about_height;
        int x = this->screen_width/2 - w/2;
        int y = 0;
        glViewport(x_off + x, y, w, h);
        o = this->about_shader;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        glDisable(GL_DEPTH_TEST);
        o->draw();
        glEnable(GL_DEPTH_TEST);
    }

    // display game over
    if ( this->show_game_over ) {
        int w = this->screen_width * 3/4.;
        int h = w * img_game_over_height * 1. / img_game_over_width;
        int x = this->screen_width/2 - w/2;
        int y = this->screen_height/2 - h/2;
        glViewport(x_off + x, y, w, h);
        o = this->game_over_shader;
        o->bind();
        o->set_uniform("mtx_u", this->projection_orth);
        glDisable(GL_DEPTH_TEST);
        o->draw();
        glEnable(GL_DEPTH_TEST);
    }

    // physics stuff
    rotate_view(this->last_view_rot_dir);
    this->last_view_rot_dir = (*view_rot_coef_sg)() * this->last_view_rot_dir;
}
