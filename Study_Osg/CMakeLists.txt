cmake_minimum_required(VERSION 3.14)
project(Study_Osg)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenSceneGraph 3.2 REQUIRED COMPONENTS osgGA osgViewer osgDB osgUtil osg osgQt osgFX)

aux_source_directory(. OSGWidgets_SRCS)

add_library(OSGWidgets SHARED ${OSGWidgets_SRCS})

target_include_directories(OSGWidgets PUBLIC  ${OPENSCENEGRAPH_INCLUDE_DIRS} )

add_executable(Study_Osg main.cpp)

target_link_libraries(Study_Osg  ${OPENSCENEGRAPH_LIBRARIES})