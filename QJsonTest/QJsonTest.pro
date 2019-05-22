TEMPLATE = app
CONFIG -= app_bundle
#CONFIG -= qt

INCLUDEPATH += \
    cjson \
    V2HJsonData

SOURCES += main.cpp \
    cjson/cJSON.c \
    V2HJsonData/v2hjsondata.cpp

HEADERS += \
    cjson/cJSON.h \
    V2HJsonData/v2hjsondata.h
