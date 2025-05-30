QT += core network
CONFIG += console
CONFIG -= app_bundle

TARGET = PointDistanceServer
TEMPLATE = app

SOURCES += \
    main.cpp \
    mytcpserver.cpp \
    server_db.cpp

CONFIG += c++17

HEADERS += \
    mytcpserver.h \
    server_db.h
