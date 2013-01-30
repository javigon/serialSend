cmake_minimum_required(VERSION 2.4.3 FATAL_ERROR )

project(serialSend)

set(QT_MIN_VERSION 4.1.0)
find_package(Qt4 REQUIRED)


include_directories(${QT_INCLUDES} ${CMAKE_BINARY_DIR})
add_definitions(-DQT3_SUPPORT)
set(serialSendSrcs serialSend.cpp serialSendLib.cpp Log.cpp)


set(exeType)
set(binInstallDir . )

add_executable(serialSend ${exeType} ${serialSendSrcs} ${uiHeaders})
target_link_libraries(serialSend ${QT_QT3SUPPORT_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

install(TARGETS serialSend DESTINATION ${binInstallDir} )

find_program(KDECONFIG_EXECUTABLE NAMES kde-config )


