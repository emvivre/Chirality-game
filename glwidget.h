/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <vector>
#include <QVector2D>
#include <QVector4D>
#include <QQuaternion>

class Renderable;
class QMouseEvent;
class SignalGenerator;

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    SignalGenerator* offset_sg;
    SignalGenerator* view_rot_coef_sg;
    int screen_width, screen_height;
    QMatrix4x4 projection;
    QMatrix4x4 projection_orth;
    std::vector<Renderable*> objects_to_render_left;
    std::vector<Renderable*> objects_to_render_right;    
    Renderable* border_shader;
    Renderable* button_shader_yes;
    Renderable* button_shader_no;    
    Renderable* button_shader_prev;
    Renderable* button_shader_next;
    Renderable* button_shader_prev_gray;
    Renderable* button_shader_next_gray;
    Renderable* button_shader_reset;
    std::vector<Renderable*> molecule_img_shader;
    std::vector<Renderable*> level_img_shader;
    Renderable* white_shader;
    Renderable* about_shader;
    Renderable* question_mark_shader;
    Renderable* game_over_shader;
    bool show_game_over;
    bool show_about;
    int object_current;   
    float fov;    
    QPoint mouseLastPoint;
    bool mouseLastPointInitialized;
    bool is_view_rotate_left;
    QQuaternion view_rotate_left;
    QQuaternion view_rotate_right;    
    QVector2D last_view_rot_dir;
    int current_level_max;
    std::vector<std::pair<QQuaternion, QQuaternion> > rotation_saved;

    int y_to_object_i(int y);
    void update_view_rotation_left_right();
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void rotate_view(QVector2D v);
    void reset_game();
public:
    GLWidget(QGLWidget *parent = 0);
};


#endif // GLWIDGET_H
