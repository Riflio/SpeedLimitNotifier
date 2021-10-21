TEMPLATE = lib
CONFIG += dll
QT += core androidextras positioning remoteobjects multimedia
TARGET = SpeedLimitService

SOURCES += \
    main.cpp \
    servicecore.cpp

HEADERS += \
    servicecore.h \
    servicemessenger.h \
    servicemessenger.rep.h

REPC_SOURCE += servicemessenger.rep

RESOURCES += \
    req.qrc
