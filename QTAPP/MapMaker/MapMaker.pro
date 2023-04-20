QT       += core gui printsupport concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 \
          thread

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ./App/ProjectManager.cpp \
    ./App/MapManager.cpp \
    ./Library/ImageBase.cpp

HEADERS += \
    mainwindow.h \
    ./App/ProjectManager.h \
    ./App/MapManager.h \
    ./App/DataManager.hpp \
    ./App/HeightMap.hpp \
    ./App/ReliefMap.hpp \
    ./App/RiverMap.hpp \
    ./App/Utilities.hpp \
    ./App/Noise.hpp \
    ./Library/image_ppm.h \
    ./Library/ImageBase.h \
    ./Library/PerlinNoise.hpp \
    ./App/PointOfInterestMap.hpp \
    ./App/ClimateMap.hpp \
    ./App/BiomeMap.hpp \
    ./App/FinalMap.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
