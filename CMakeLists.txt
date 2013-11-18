cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(PROJECT TideGraph)


set (SOURCES
main.cpp
maingraphicswidget.cpp
mainscene.cpp
mainview.cpp
mainwindow.cpp
tidedatacache.cpp
tidegraphwidget.cpp )

set (HEADERS
maingraphicswidget.h
mainscene.h
mainview.h
mainwindow.h
rl_ptr.h
tidedatacache.h
tidegraphwidget.h )

set (RESOURCES
tideGraph.qrc )

project(${PROJECT})

include_directories (../)

find_package(Qt4 REQUIRED)
include (${QT_USE_FILE})

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_add_resources (QRC_SOURCES ${RESOURCES})

# target_link_libraries(${PROJECT} ${QT_LIBRARIES})

add_executable(${PROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES})
target_link_libraries(${PROJECT} ${QT_LIBRARIES})
