QT += core network sql

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    server.cpp \
    database.cpp \
    userdatabase.cpp \
    graph.cpp

HEADERS += \
    server.h \
    database.h \
    userdatabase.h \
    graph.h
