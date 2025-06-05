QT += core network
  CONFIG += c++11
  CONFIG += console  # Убедитесь, что эта строка присутствует
  TARGET = client
  SOURCES += main.cpp client.cpp
  HEADERS += client.h
