QT = core
QT -= gui
QT += testlib
CONFIG += c++17 cmdline
INCLUDEPATH += $$PWD/../ParseServer/Server
QT += network testlib

# Исходные файлы
SOURCES += \
    main.cpp \
    vigenereciphertests.cpp \
    ../ParseServer/Server/server.cpp \
    ../ParseServer/Server/graph.cpp \
    ../ParseServer/Server/database.cpp

# Заголовочные файлы
HEADERS += \
    vigenereciphertests.h \
    ../ParseServer/Server/server.h \
    ../ParseServer/Server/graph.h \
    ../ParseServer/Server/database.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
