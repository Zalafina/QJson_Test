TEMPLATE = app
CONFIG -= app_bundle
#CONFIG -= qt

INCLUDEPATH += \
    cjson

SOURCES += main.cpp \
    cjson/cJSON.c

HEADERS += \
    cjson/cJSON.h
