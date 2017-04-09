/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "mainwindow.h"
#include <QApplication>
#include "glwidget.h"
#include <QHBoxLayout>
#include <ctime>

MainWindow::MainWindow() {
    GLWidget* glw = new GLWidget();
    setCentralWidget(glw);
}

int main(int argc, char **argv)
{
    srand(time(0));
    QApplication app(argc, argv);
    MainWindow m;   
    m.resize(1024, 600);
    m.show();
    app.exec();
    return 0;
}
