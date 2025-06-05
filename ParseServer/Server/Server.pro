QT += core network
  CONFIG += c++11
  CONFIG += console  # Убедитесь, что эта строка присутствует
  TARGET = server
  SOURCES += main.cpp graph.cpp server.cpp database.cpp
  HEADERS += graph.h server.h database.h
