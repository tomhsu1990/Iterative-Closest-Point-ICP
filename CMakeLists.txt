CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ICP)

#release mode
SET(CMAKE_BUILD_TYPE RELEASE)

#############################
# Qt ########################
#############################
#FIND_PACKAGE(Qt4 REQUIRED)
#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})
#SET(CMAKE_INCLUDE_CURRENT_DIR ON)
#QT4_WRAP_CPP(SENSOR_HEADERS_MOC Sensors/Xtions/xtioncapture.h)

#############################
# SickToolbox ###############
#############################
#FIND_PACKAGE(SickToolbox REQUIRED)
#INCLUDE_DIRECTORIES(/usr/local/include)
#FIND_LIBRARY(${SICKLIB} libsicklms-1.0 libpth /usr/local/lib /usr/lib)
#FIND_LIBRARY(${SICKLIB} libsicklms-1.0 /usr/local/lib)
#LINK_DIRECTORIES(/usr/local/lib)

#using pthread in sicktoolbox
#FIND_PACKAGE(Threads REQUIRED)

#############################
# OpenCV ####################
#############################
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#############################
# OpenNI2 ###################
#############################
#SET(OPENNI2_DIR /home/chsu/Desktop/OpenNI-Linux-x86-2.4)
#INCLUDE_DIRECTORIES(${OPENNI2_DIR}/Include)
#LINK_DIRECTORIES(${OPENNI2_DIR}/Redist/)
#LINK_LIBRARIES(${OPENNI2_DIR}/Redist)

#############################
# ANN #######################
#############################
SET(ANN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ann_1.1.2)
INCLUDE_DIRECTORIES(${ANN_DIR}/include)
LINK_DIRECTORIES(${ANN_DIR}/lib)
LINK_LIBRARIES(${ANN_DIR}/lib)

#current files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(ICP
ICP.cpp
ICPHelper.hpp
ICPHelper.cpp
ANNHelper.hpp
ANNHelper.cpp
RandomGenerator.hpp
RandomGenerator.cpp
)

TARGET_LINK_LIBRARIES(ICP
${OpenCV_LIBS}
opencv_calib3d
opencv_highgui
opencv_imgproc
opencv_features2d
opencv_flann
opencv_core
ANN
#${OPENNI2_DIR}/Redist/libOpenNI2.so
#${QT_QTMAIN_LIBRARY}
#${QT_LIBRARIES}
#${SICKLIB}
#sicklms-1.0
#${CMAKE_THREAD_LIBS_INIT}
)
