cmake_minimum_required(VERSION 3.14)
project(triangulate)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif ()

#add_compile_options(-std=c++0x)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(triangulate main.cpp Painter.cpp Painter.h)

target_link_libraries(triangulate ${Boost_LIBRARIES} ${OpenCV_LIBS})