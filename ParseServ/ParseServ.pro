QT       += core sql network

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    ClientHandler.cpp \
    GraphProcessor.cpp \
    DatabaseManager.cpp

HEADERS += \
    ClientHandler.h \
    GraphProcessor.h \
    DatabaseManager.h
