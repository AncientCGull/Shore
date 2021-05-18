QT       += core gui widgets concurrent

CONFIG += c++11 sdk_no_version_check

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controller.cpp \
    main.cpp \
    mainwindow.cpp \
    shor.cpp

HEADERS += \
    controller.h \
    mainwindow.h \
    shor.h

FORMS += \
    mainwindow.ui

# for Qpp + OpenMP
DEFINES += NDEBUG HAS_OPENMP
QMAKE_CXXFLAGS+= -Xpreprocessor -I/usr/local/opt/libomp/include
QMAKE_LFLAGS += -openmp -L/usr/local/opt/llvm/lib
LIBS += -lomp #-L /usr/local/lib /usr/local/lib/libomp.dylib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
