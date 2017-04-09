QT += opengl
QMAKE_CXXFLAGS = -std=c++11

SOURCES += \
    main.cpp \
    renderable.cpp \
    glwidget.cpp \
    ply_binary_reader.cpp \
    regex.cpp \   
    img_button_no.c \
    img_button_yes.c \
    img_question.c \
    img_button_next.c \
    img_button_prev.c \
    molecule_ethanol.cpp \
    molecule_methane.cpp \
    molecule_R____alanine.cpp \
    molecule_R____asparagine.cpp \
    molecule_R____limonene.cpp \
    molecule_R____thalidomide.cpp \
    molecule_S____alanine.cpp \
    molecule_S____asparagine.cpp \
    molecule_S____limonene.cpp \
    molecule_S____thalidomide.cpp \
    img_molecule_alanine.cpp \
    img_molecule_asparagine.cpp \
    img_molecule_ethanol.cpp \
    img_molecule_limonene.cpp \
    img_molecule_methane.cpp \
    img_molecule_thalidomide.cpp \
    img_6_over_6.c \
    img_5_over_6.c \
    img_4_over_6.c \
    img_3_over_6.c \
    img_1_over_6.c \
    img_2_over_6.c \
    img_button_next_gray.c \
    img_button_prev_gray.c \
    img_white.c \
    img_about.c \
    img_question_mark.c \
    img_game_over.c \
    img_button_reset.c \
    source_code.c

HEADERS += \
    glwidget.h \
    mainwindow.h \
    renderable.h \
    ply_binary_reader.h \
    regex.hh \   
    img_button_no.h \
    img_button_yes.h \
    img_question.h \
    img_button_next.h \
    img_button_prev.h \
    molecule_ethanol.h \
    molecule_methane.h \
    molecule_R____alanine.h \
    molecule_R____asparagine.h \
    molecule_R____limonene.h \
    molecule_R____thalidomide.h \
    molecule_S____alanine.h \
    molecule_S____asparagine.h \
    molecule_S____limonene.h \
    molecule_S____thalidomide.h \
    img_molecule_alanine.h \
    img_molecule_asparagine.h \
    img_molecule_ethanol.h \
    img_molecule_limonene.h \
    img_molecule_methane.h \
    img_molecule_thalidomide.h \
    img_6_over_6.h \
    img_4_over_6.h \
    img_5_over_6.h \
    img_3_over_6.h \
    img_1_over_6.h \
    img_2_over_6.h \
    img_button_next_gray.h \
    img_button_prev_gray.h \
    img_white.h \
    img_about.h \
    img_question_mark.h \
    img_game_over.h \
    img_button_reset.h \
    source_code.h

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/version.xml \
    android/res/values-ja/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    img_molecule_alanine.png \
    img_molecule_asparagine.png \
    img_molecule_ethanol.png \
    img_molecule_limonene.png \
    img_molecule_methane.png \
    img_molecule_thalidomide.png

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
