TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
        $$PWD\source\*.hpp

SOURCES += \
        $$PWD\source\*.cpp \
        $$PWD\examples\*.cpp \
        $$PWD\unit_tests.cpp
