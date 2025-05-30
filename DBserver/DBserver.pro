QT += core network

CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
    DBServer.cpp \
    Graph.cpp \
    Vigenere.cpp \
    main.cpp

HEADERS += \
    DBServer.h \
    Graph.h \
    Types.h \
    Vigenere.h
